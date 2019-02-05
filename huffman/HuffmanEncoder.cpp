#include "HuffmanEncoder.h"
#include <string>
#include <fstream>
#include <iostream>
#include "misc.h"
#include "HuffmanTree.h"

HuffmanEncoder::HuffmanEncoder()
{
}

HuffmanEncoder::~HuffmanEncoder()
{
}

void HuffmanEncoder::setInputFile(std::string path)
{
	inputFilePath = path;
}

void HuffmanEncoder::setOutputFile(std::string path)
{
	outputFilePath = path;
}

bool HuffmanEncoder::encode() {
	int charStats[maxAlphabetSize];

	std::ifstream::pos_type size;
	char latestByte;

	for (int j = 0; j < maxAlphabetSize; j++)
		charStats[j] = 0; // Initialize statistics

	using std::cout;
	using std::endl;
	using std::ios;

	std::ifstream file(inputFilePath, ios::in | ios::binary | ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		file.seekg(0, ios::beg);
		cout << "File loaded." << endl;

		cout << "Gathering statistics..." << endl;

		while (file.get(latestByte)) {
			charStats[char2index(latestByte)]++;
		}

		//cout << "Number of occurrences for each character: " << endl;
		//for (int i = 0; i < maxAlphabetSize; i++) {
		//	cout << index2char(i) << ": " << charStats[i] << endl;
		//}

		file.close();

	}
	else {
		cout << "Unable to open file." << endl;
		return false;
	}

	HuffmanTree tree;
	tree.build(charStats);

	CodeTable codeTable = CodeTable();
	tree.writeCodeLengths(codeTable);

	cout << "Setting up code table" << endl;
	codeTable.setupCodes();
	//codeTable.print();

	file.open(inputFilePath, ios::in | ios::binary | ios::ate);

	double inSize, outSize;

	if (file.is_open()) {

		size = file.tellg();
		inSize = (double)size;
		file.seekg(0, ios::beg);
		
		cout << "Writing to file... " << endl;

		ByteBuffer buffer = ByteBuffer();
		std::ofstream outputStream(outputFilePath, ios::binary);
		codeTable.writeSideInfo(buffer, outputStream);

		while (file.get(latestByte))
		{
			codeTable.writeCode(char2index(latestByte),
				buffer, outputStream);
		}
		codeTable.writeEOF(buffer, outputStream);
		outSize = (double)(outputStream.tellp());

		file.close();

		outputStream.close();

	}
	else {
		cout << "Unable to open file";
		return false;
	}

	cout << endl << "Finished writing to file. " << endl;
	cout << "Bits per character: " << outSize * 8 / inSize << endl;
	cout << "Compression ratio: " << inSize / outSize << endl;

	return true;
}
