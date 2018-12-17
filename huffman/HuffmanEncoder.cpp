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

void HuffmanEncoder::setInputFile(string path)
{
	inputFilePath = path;
}

void HuffmanEncoder::setOutputFile(string path)
{
	outputFilePath = path;
}

bool HuffmanEncoder::encode() {
	int charStats[maxAlphabetSize];

	ifstream::pos_type size;
	char * memblock;

	for (int j = 0; j < maxAlphabetSize; j++)
		charStats[j] = 0; // Initialize statistics

	ifstream file(inputFilePath, ios::in | ios::binary | ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		memblock = new char[(unsigned int)size];
		file.seekg(0, ios::beg);
		file.read(memblock, size);
		file.close();

		cout << "File loaded." << endl;

		cout << "Gathering statistics..." << endl;

		//Count number of occurrences for every symbol
		for (int j = 0; j < size; j++)
		{
			charStats[(unsigned char)memblock[j]]++;
		}

		delete[] memblock;
	}
	else {
		cout << "Unable to open file." << endl;
		return false;
	}

	HuffmanTree tree;
	tree.build(charStats);

	CodeTable codeTable = CodeTable();
	tree.writeCodes(codeTable);

	cout << "Standardizing code table" << endl;
	//codeTable.print();
	codeTable.standardizeAndSetEOF();
	//codeTable.print();
	

	file.open(inputFilePath, ios::in | ios::binary | ios::ate);

	double inSize, outSize;

	if (file.is_open()) {

		size = file.tellg();
		inSize = (double)size;
		memblock = new char[(int)size];
		file.seekg(0, ios::beg);
		file.read(memblock, size);
		file.close();

		cout << "Number of chars in original file: " << (int)size << endl;
		cout << "Writing to file... " << endl;

		ByteBuffer buffer = ByteBuffer();
		ofstream outputStream(outputFilePath, ios::binary);
		codeTable.writeSideInfo(buffer, outputStream);

		// Encode and write to file
		for (int j = 0; j < size; j++)
		{
			//if (j < 10) {
			//	cout << "writing code for symbol " << char2index(memblock[j]) <<
			//		" (" << memblock[j] << ")" << endl;
			//}
			
			codeTable.writeCode(char2index(memblock[j]),
				buffer, outputStream);
		}
		codeTable.writeEOF(buffer, outputStream);
		outSize = (double)(outputStream.tellp());

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
