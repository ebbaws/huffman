#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include "misc.h"
#include "HuffmanTree.h"
#include "HuffmanEncoder.h"
#include "ByteBuffer.h"
using namespace std;

string infilename = "C:\\learn\\cantrbry\\asyoulik.txt";
string outfilename = "output.huff";

char myLine[100];

bool encode(string & inputFilePath, string & outputFilePath) {
	HuffmanEncoder encoder;
	encoder.setInputFile(inputFilePath);
	encoder.setOutputFile(outputFilePath);
	
	return encoder.encode();
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

int main (int argc, char *argv[])  {

	if (argc > 1) {
		int fileNameLength = strlen(argv[1]);

		if (fileNameLength > 0) {
			string inputFile(argv[1], fileNameLength);
			cout << "File name: " << inputFile << endl;
			string outputFile = "output.huff";
			if (encode(inputFile, outputFile)) {
				cout << "Name of encoded file: " << outputFile << endl;
			}
			else {
				cout << "Encoding failed" << endl;
			}


		}
		else {
			cout << "Please specify file name" << endl;
		}

	}

	return 0;

}
