#ifndef CODETABLE_H
#define CODETABLE_H

#include "misc.h"
#include "ByteBuffer.h"
class CodeTable
{
private:
	unsigned long long codes[maxAlphabetSize];
	int lengths[maxAlphabetSize];
	unsigned long long codeEOF = 0;
	int lengthEOF = 0;
	int leastLikelyIdx = -1;
	void printLine(int index);

public:
	CodeTable(); 
	~CodeTable();
	void print();
	void standardizeAndSetEOF();
	void writeSideInfo(ByteBuffer& buffer, std::ofstream& stream);
	void writeCode(int index, ByteBuffer& buffer, std::ofstream& stream);
	void writeEOF(ByteBuffer& buffer, std::ofstream& stream);

	void setCode(int index, unsigned long long code, int codeLength);
};

#endif