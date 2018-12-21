#include "CodeTable.h"
#include <iostream>
using namespace std;

CodeTable::CodeTable()
{
	for (int i = 0; i < maxAlphabetSize; i++) {
		codes[i] = 0;
		lengths[i] = 0;
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

// to modify this for the new code representation,
// just change type of input code to vector<bool>
void CodeTable::setCode(int index, unsigned long long code, int codeLength) {
	codes[index] = code;
	lengths[index] = codeLength;
}

void CodeTable::setLength(int index, int codeLength) {
	lengths[index] = codeLength;
}

// Modify the codes to a standard appearance, so that we only
// need the code lengths as side info for decoding.
// Also, add a valid end-of-file (EOF) code to this table.
void CodeTable::standardizeAndSetEOF() {
	mySort(lengths, sortedIndexes, maxAlphabetSize, true);
	sortedIndexesAvailable = true;

	int currentIdx = 0;
	while (lengths[sortedIndexes[currentIdx]] == 0) currentIdx++;

	unsigned long long code = 0;
	codes[sortedIndexes[currentIdx]] = code;

	int thisLength = lengths[sortedIndexes[currentIdx]];
	int oldLength;

	currentIdx++;

	// If we want each code represented as a vector<char>
	// we have to find a way to do those shift operations
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
	int leastLikelyIdx = sortedIndexes[255]; // !!!!!!

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

// For new code representation the addBits method in ByteBuffer
// will have to change...
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
	// Attempt to open file
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

		standardizeAndSetEOF();
		return true;
	}
	else {
		return false;
	}

}

int CodeTable::matchCode(unsigned long long code, int codeLength)
{
	if ((code == codeEOF) && (codeLength == lengthEOF))
		return 256;

	int matchIdx = -1;

	if (!sortedIndexesAvailable) {
		mySort(lengths, sortedIndexes, maxAlphabetSize, true);
		sortedIndexesAvailable = true;
	}

	// maybe we should save actual alphabet size somewhere so
	// we don't have to step through a bunch of zeros here,
	// or think of some other more efficient solution
	// (maybe create some kind of "lookup table" for the lengths?)
	int currentIdx = 0;
	while (lengths[sortedIndexes[currentIdx]] < codeLength)
		currentIdx++;

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
	if (!sortedIndexesAvailable) {
		mySort(lengths, sortedIndexes, maxAlphabetSize, true);
		sortedIndexesAvailable = true;
	}

	return lengths[sortedIndexes[maxAlphabetSize-1]];
}
