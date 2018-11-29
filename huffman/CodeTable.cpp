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

void CodeTable::setCode(int index, unsigned long long code, int codeLength) {
	codes[index] = code;
	lengths[index] = codeLength;
}

// Modify the codes to a standard appearance, so that we only
// need the code lengths as side info for decoding.
// Also, add a valid end-of-file (EOF) code to this table.
void CodeTable::standardizeAndSetEOF() {
	int indexes[maxAlphabetSize];
	mySort(lengths, indexes, maxAlphabetSize, true);

	int currentIdx = 0;
	while (lengths[indexes[currentIdx]] == 0) currentIdx++;

	unsigned long long code = 0;
	codes[indexes[currentIdx]] = code;

	int thisLength = lengths[indexes[currentIdx]];
	int oldLength;

	currentIdx++;

	for (currentIdx; currentIdx < maxAlphabetSize; currentIdx++)
	{
		oldLength = thisLength;
		thisLength = lengths[indexes[currentIdx]];
		code = code + 1;
		// could the below line be changed to left shift instead?
		// also that cast is not good
		code = code * (int)pow(2.0, (thisLength - oldLength)); 
		codes[indexes[currentIdx]] = code;
	}

	// Modify the tree slightly to include an EOF code
	// by splitting the "least likely" node
	leastLikelyIdx = indexes[255];
	codes[leastLikelyIdx] = 2 * codes[leastLikelyIdx];
	lengths[leastLikelyIdx]++;
	codeEOF = codes[leastLikelyIdx] + 1;
	lengthEOF = lengths[leastLikelyIdx];
}

void CodeTable::writeSideInfo(ByteBuffer & buffer, std::ofstream& stream)
{
	// Write code lengths as they were before the EOF code was added
	for (int i = 0; i < maxAlphabetSize; i++) {
		int currentLength = lengths[i];
		if (i == leastLikelyIdx)
			currentLength = currentLength - 1;
		buffer.addBits(lengths[i], 8); // implicit cast -- dunno about this
		buffer.writeBytes(stream);
	}

	// Write index of least likely node as well, so the EOF code
	// can be reconstructed
	buffer.addBits(leastLikelyIdx, 8);

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