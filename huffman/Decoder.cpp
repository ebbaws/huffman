#include "Decoder.h"
#include "misc.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

Decoder::Decoder()
{
}


Decoder::~Decoder()
{
}

void Decoder::setInputFile(string path)
{
	inputFilePath = path;
}

void Decoder::setOutputFile(string path)
{
	outputFilePath = path;
}


int Decoder::getBit(char bits, int bitPos) {
	if (bitPos > -1 && bitPos < 8) {
		char bitmask = 128 >> bitPos;
		return ((bits & bitmask) == 0) ? 0 : 1;
	}
	else {
		return -1;
	}
}

// Get index of next code
int Decoder::getNextCode(ifstream & stream)
{
	int minLength = codeTable.getMinLength();
	int maxLength = codeTable.getMaxLength();

	int currentLength = 0;
	unsigned long long currentCode = 0;

	while (currentLength<=maxLength) {

		// Get new byte from stream if necessary
		if (currentBitPosition > 7) {
			//cout << "Reading new byte from stream" << endl;
			if (stream.get(latestByte)) {
				currentBitPosition = 0;
			}
			else {
				return -1;
			}
		}

		// Add next bit to code
		currentCode = (currentCode << 1) + getBit(latestByte, currentBitPosition);
		currentBitPosition++;
		currentLength++;

		//cout << "Code: " << currentCode << ", " << currentLength << endl;

		if (currentLength >= minLength) {
			int matchIdx = codeTable.matchCode(currentCode, currentLength);
			if (matchIdx >= 0) return matchIdx;
		}

	}
	return -1;
}

bool Decoder::decode()
{
	bool res = codeTable.initializeFromFileHeader(inputFilePath);
	if (!res) return false;

	cout << "Initialized code table for decode" << endl;

	ifstream file(inputFilePath, ios::in | ios::binary | ios::ate);
	if (file.is_open()) {

		ofstream fout(outputFilePath, ios::binary);

		currentBitPosition = 0;

		// Set read position to beginning of file body
		// (first 256 bytes are code table data)
		file.seekg(256);
		if (file.get(latestByte)) {
			//cout << "Read " << char2index(latestByte) << " from stream" << endl;
		}
		else {
			cout << "Something went wrong" << endl;
			file.close();
			return false;
		}

		cout << "Starting decoding" << endl;
		int nextCode = 0;
		int j = 0;
		while (true) {
			nextCode = getNextCode(file);

			if (nextCode<0||nextCode>256) {
				cout << "Could not read valid code in iteration " << j << endl;
				break;
			}
			else if (nextCode == 256) {
				cout << "Read end-of-file code" << endl;
				break;
			}
			else {
				char decodedChar = index2char(nextCode);
				fout.write(&decodedChar, 1);
			}

			j++;

			//if (j % 1000 == 0) {
			//	cout << "decode iteration #" << j << endl;
			//}

		}

		fout.close();
		file.close();

	} else return false;

	return true;
}

