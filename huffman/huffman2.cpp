#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include "misc.h"
#include "HuffmanTree.h"
#include "HuffmanEncoder.h"
#include "ByteBuffer.h"
#include "Decoder.h"
using namespace std;

bool encode(string & inputFilePath, string & outputFilePath) {
	HuffmanEncoder encoder;
	encoder.setInputFile(inputFilePath);
	encoder.setOutputFile(outputFilePath);
	
	return encoder.encode();
}

bool decode(string & inputFilePath, string & outputFilePath) {
	
	Decoder decoder;
	decoder.setInputFile(inputFilePath);
	decoder.setOutputFile(outputFilePath);

	return decoder.decode();

}

bool test(string & inputFilePath) {
	string encodedFilePath = "encoded.huff";
	string decodedFilePath = "decoded.huff";
	if (encode(inputFilePath, encodedFilePath)&&
		decode(encodedFilePath, decodedFilePath)) {

		return (compareFiles(inputFilePath, decodedFilePath));

	}
	else {
		cout << "Couldn't finish test" << endl;
		return false;
	}

}


void printUsageInstructions() {
	cout << "Please specify an input file name" << endl;
	cout << "For decode mode, write -decode" << endl;
	cout << "Debug options: -test, -testsort" << endl;
	cout << "Output file name (for decode or encode mode)" <<
		" can also be specified after input file name" << endl;
}

int main (int argc, char *argv[])  {
	string inputFileName = "";
	string outputFileName = "output.huff";

	bool decodeMode = false;
	bool testMode = false;
	bool testSortMode = false;
	bool result = false;

	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			string argString(argv[i], strlen(argv[i]));

			if (argv[i][0] == '-') {
				string decodeString = "-decode";
				string testString = "-test";
				string testSortString = "-testsort";
				
				if (!argString.compare(decodeString)) {
					decodeMode = true;
				} else if (!argString.compare(testString)) {
					testMode = true;
				}
				else if (!argString.compare(testSortString)) {
					testSortMode = true;
				}
				else {
					cout << "Invalid option" << endl;
				}

			} else {
				if (inputFileName == "") { 
					inputFileName = argString;
				}
				else {
					outputFileName = argString;
				}
			
			}

		}
	}

	if (testSortMode) {
		testSort();
		return 0;
	}

	if (inputFileName == "") {
		printUsageInstructions();
	}
	else if (testMode) {
		cout << "Input file name: " << inputFileName << endl;
		cout << "Starting the test" << endl;
		result = test(inputFileName);
	}
	else {
		cout << "Input file name: " << inputFileName << endl;
		cout << "Output file name: " << outputFileName << endl;
		if (decodeMode) {
			cout << "Starting decoding" << endl;
			result = decode(inputFileName, outputFileName);
		}
		else {
			cout << "Encode mode" << endl;
			result = encode(inputFileName, outputFileName);
		}
	}

	return result ? 0 : -1;

}
