#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include "misc.h"
#include "HuffmanTree.h"
#include "ByteBuffer.h"
using namespace std;

string infilename = "C:\\learn\\cantrbry\\asyoulik.txt";
string outfilename = "asyoulik.txt.huff";

char myLine[100];

// currently we are getting a <1 compression ratio so
// clearly this is not working correctly :/
// the code table is looking weird
// update: the huffman tree is not looking the same as the original
// maybe we should check that the sorting is working first
// ok the sorting appears to work at least for a small example

int encode()
{
	int charStats[maxAlphabetSize];

	ifstream::pos_type size;
	char * memblock;

	for (int j = 0; j < maxAlphabetSize; j++)
		charStats[j] = 0; // Initialize statistics

	// Open the input file to gather statistics
	ifstream file(infilename, ios::in | ios::binary | ios::ate);
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
		return -1;
	}

	cout << "Building code tree..." << endl;

	HuffmanTree tree;
	tree.build(charStats);

	tree.print();

	CodeTable codeTable = CodeTable();
	tree.writeCodes(codeTable);

	//codeTable.print();

	cout << "Standardizing code table" << endl;

	codeTable.standardizeAndSetEOF();
	
	//codeTable.print();

	file.open(infilename, ios::in | ios::binary | ios::ate);

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
		ofstream outputStream(outfilename, ios::binary);
		codeTable.writeSideInfo(buffer, outputStream);

		// Encode and write to file
		for (int j = 0; j < size; j++)
		{
			codeTable.writeCode(char2index(memblock[j]),
				buffer, outputStream);
		}
		codeTable.writeEOF(buffer, outputStream);
		outSize = (double)(outputStream.tellp());

		outputStream.close();

	}
	else {
		cout << "Unable to open file";
		return -1;
	}

	cout << endl << "Finished writing to file. " << endl;
	cout << "Bits per character (full bytes): " << outSize * 8 / inSize << endl;
	cout << "Compression ratio: " << inSize / outSize << endl;

	return 0;
	//cin.getline(myLine, 100);

}

void testSort() {
	int values[10] = { 19, -4, 6, 23, 108, -11, 78, 90, 24, 35 };
	int indexes[10];
	mySort(values, indexes, 10);

	for (int i = 0; i < 10; i++) {
		cout << values[indexes[i]] << " ";
	}
}

void testByteBuffer() {
	string fileName = "testfile";
	ByteBuffer buffer = ByteBuffer();
	ofstream outputStream(fileName, ios::binary);
	buffer.addBits(255, 8);
	buffer.print();
	buffer.addBits(0, 4);
	buffer.print();
	buffer.addBits(63, 6);
	buffer.print();
	buffer.addBits(0, 1);
	buffer.print();
	buffer.addBits(3, 2);
	buffer.print();
	buffer.addBits(2, 3);
	buffer.print();
	
	unsigned long long superLongCode = 0x97ffffffff;
	
	buffer.addBits(superLongCode, 40);
	buffer.print();

	buffer.writeBytes(outputStream);
	outputStream.close();
	buffer.print();

}

int main ()  {

	return encode();
	//testSort();
}
