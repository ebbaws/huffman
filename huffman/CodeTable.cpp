#include "CodeTable.h"
#include <iostream>
using namespace std;

CodeTable::CodeTable()
{
	for (int i = 0; i < maxAlphabetSize; i++) {
		codes[i] = 0;
		lengths[i] = 0;
		codeLengthLookup[i] = -1;
	}
}

CodeTable::~CodeTable()
{
}

void CodeTable::printLine(int idx) {
	if (lengths[idx] != 0) {
		cout << index2char(idx) << ": " << codes[idx] <<
			" (" << lengths[idx] << " bits)" << endl;
	}
}

void CodeTable::print() {
	cout << "helo i am table " << endl;
	for (int i = 0; i < maxAlphabetSize; i++) {
		printLine(i);
	}
}

void CodeTable::setCode(int index, unsigned long long code, int codeLength) {
	codes[index] = code;
	lengths[index] = codeLength;
}

void CodeTable::setLength(int index, int codeLength) {
	lengths[index] = codeLength;
}

void CodeTable::setupCodeLengthLookup() {

	int currentLength = -1;

	for (int i = 0; i < maxAlphabetSize; i++) {
		if (lengths[sortedIndexes[i]] != currentLength) {
			if (currentLength == 0) {
				minLength = lengths[sortedIndexes[i]];
			}

			currentLength = lengths[sortedIndexes[i]];
			codeLengthLookup[currentLength] = i;
		}
	}

	maxLength = currentLength;

	//cout << "Code length lookup table" << endl;
	//for (int i = 0; i < maxAlphabetSize; i++) {
	//	cout << i << ": " << codeLengthLookup[i] << ", ";
	//}

	//cout << endl;

	//cout << "Min code length: " << minLength << endl;
	//cout << "Max code length: " << maxLength << endl;

	readyForDecode = true;

}

// Set codes to a standard appearance based on code lengths.
// Also, add a valid end-of-file (EOF) code to this table.
void CodeTable::setupCodes() {
	if (!sortedIndexesAvailable) {
		mySort(lengths, sortedIndexes, maxAlphabetSize, true);
		sortedIndexesAvailable = true;
	}

	int currentIdx = 0;
	while (lengths[sortedIndexes[currentIdx]] == 0) currentIdx++;

	unsigned long long code = 0;
	codes[sortedIndexes[currentIdx]] = code;

	int thisLength = lengths[sortedIndexes[currentIdx]];
	int oldLength;

	currentIdx++;

	for (currentIdx; currentIdx < maxAlphabetSize; currentIdx++)
	{
		oldLength = thisLength;
		thisLength = lengths[sortedIndexes[currentIdx]];
		code = code + 1;
		code = code << (thisLength - oldLength);
		codes[sortedIndexes[currentIdx]] = code;
	}

	// Modify the tree slightly to include an EOF code
	// by splitting the "least likely" node
	int leastLikelyIdx = sortedIndexes[255];

	codes[leastLikelyIdx] = 2 * codes[leastLikelyIdx];
	lengths[leastLikelyIdx]++;
	codeEOF = codes[leastLikelyIdx] + 1;
	lengthEOF = lengths[leastLikelyIdx];
	//print();

}

void CodeTable::writeSideInfo(ByteBuffer & buffer, std::ofstream& stream)
{
	// Write code lengths as they were before the EOF code was added
	for (int i = 0; i < maxAlphabetSize; i++) {
		int currentLength = lengths[i];
		if (i == sortedIndexes[255])
			currentLength = currentLength - 1;

		// the buffer is probably kind of unnecessary?
		buffer.addBits(currentLength, 8); // dunno about this
		buffer.writeBytes(stream);
	}

}

void CodeTable::writeCode(int index, ByteBuffer & buffer, std::ofstream & stream)
{
	buffer.addBits(codes[index], lengths[index]);
	buffer.writeBytes(stream);
}

void CodeTable::writeEOF(ByteBuffer & buffer, std::ofstream & stream)
{
	buffer.addBits(codeEOF, lengthEOF);
	buffer.writeBytes(stream, true);
}

// Reconstruct table from header of encoded file
bool CodeTable::initializeFromFileHeader(string & fileName) {
	decodeMode = true;
	ifstream file(fileName, ios::in | ios::binary | ios::ate);
	char nextByte;
	if (file.is_open()) {
		file.seekg(0, ios::beg);
		int i = 0;
		while (file.tellg() < maxAlphabetSize) {
			file.get(nextByte);
			lengths[i] = char2index(nextByte);
			//cout << i << ":\tReceived " << char2index(nextByte) <<
			//	" from file stream" << endl;
			i++;
		}
		
		file.close();

		setupCodes();
		setupCodeLengthLookup();
		return true;
	}
	else {
		return false;
	}

}

int CodeTable::matchCode(unsigned long long code, int codeLength)
{
	if (!readyForDecode) return -1;

	if ((code == codeEOF) && (codeLength == lengthEOF))
		return 256;

	int currentIdx = codeLengthLookup[codeLength];

	if (currentIdx == -1) return -1; // Table has no codes of this length

	int matchIdx = -1;
	
	while ((lengths[sortedIndexes[currentIdx]] == codeLength)
			&&(matchIdx==-1)) {
		if (codes[sortedIndexes[currentIdx]] == code) {
			matchIdx = sortedIndexes[currentIdx];
			break; // prevent out-of-range issues
		}
		currentIdx++;
	}

	return matchIdx;
}

int CodeTable::getMinLength()
{
	if (readyForDecode) {
		return minLength;
	}

	if (!sortedIndexesAvailable) {
		mySort(lengths, sortedIndexes, maxAlphabetSize, true);
		sortedIndexesAvailable = true;
	}

	int currentIdx = 0;
	while (lengths[sortedIndexes[currentIdx]] == 0)
		currentIdx++;

	return lengths[sortedIndexes[currentIdx]];
}

int CodeTable::getMaxLength()
{
	if (readyForDecode) {
		return maxLength;
	}

	if (!sortedIndexesAvailable) {
		mySort(lengths, sortedIndexes, maxAlphabetSize, true);
		sortedIndexesAvailable = true;
	}

	return lengths[sortedIndexes[maxAlphabetSize-1]];
}
