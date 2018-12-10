#ifndef DECODER_H
#define DECODER_H
#include <iostream>
#include "CodeTable.h"

class Decoder
{
private:
	std::string inputFilePath;
	std::string outputFilePath;
	char latestByte = '&';
	int currentBitPosition;
	CodeTable codeTable;
	int getBit(char bits, int bitPos);
	int getNextCode(std::ifstream & stream);

public:
	Decoder();
	~Decoder();
	void setInputFile(std::string path);
	void setOutputFile(std::string path);
	bool decode();

};

#endif

