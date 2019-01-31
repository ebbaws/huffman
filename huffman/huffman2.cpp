#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include "misc.h"
#include "HuffmanTree.h"
#include "HuffmanEncoder.h"
#include "ByteBuffer.h"
#include "Decoder.h"
#include "EntropyCalculator.h"
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

bool calcEntropy(string & inputFilePath, int order) {
	EntropyCalculator calculator;
	double entropy;

	if (order > 0) {
		entropy = calculator.calcHigherOrderEntropy(inputFilePath, order);
	}
	else {
		entropy = calculator.calcMemorylessEntropy(inputFilePath);
	}
	
	if (entropy < 0.0) {
		cout << "Entropy calculation failed" << endl;
		return false;
	}
	else {
		cout << "Entropy: " << entropy << endl;
		return true;
	}

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
	cout << "Default mode: encode" << endl;
	cout << "For decode mode, write -decode" << endl;
	cout << "For entropy calculation mode, write -entropy" << endl;
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
	bool entropyMode = false;
	int entropyOrder = 0;

	// Parse arguments

	bool waitingForEntropyOrder = false;
	bool modeSpecified = false;

	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			string argString(argv[i], strlen(argv[i]));

			if (waitingForEntropyOrder) {
				// Determine if this argument is a number.
				// If so, extract it and assign it to entropyOrder
				int order = stringToPositiveInt(argString);
				waitingForEntropyOrder = false;
				if (order < 0) {
					cout << "No entropy order specified" << endl;
				}
				else {
					cout << "Entropy order: " << order << endl;
					entropyOrder = order;
					continue;
				}
			}

			if (argv[i][0] == '-') {
				string decodeString = "-decode";
				string testString = "-test";
				string testSortString = "-testsort";
				string entropyString = "-entropy";

				if (modeSpecified) {
					cout << "Too many options specified" << endl;
					printUsageInstructions();
					return -1;
				}

				if (!argString.compare(decodeString)) {
					decodeMode = true;
				} else if (!argString.compare(testString)) {
					testMode = true;
				}
				else if (!argString.compare(testSortString)) {
					testSortMode = true;
				}
				else if (!argString.compare(entropyString)) {
					entropyMode = true;
					waitingForEntropyOrder = true;
				}
				else {
					cout << "Invalid option" << endl;
					printUsageInstructions();
					return -1;
				}
				modeSpecified = true;

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

	// Do the thing

	bool result = false;

	if (testSortMode) {
		testSort();
		return 0;
	}

	if (inputFileName == "") {
		printUsageInstructions();
		return -1;
	}
	else {
		cout << "Input file name: " << inputFileName << endl;
	}


	if (testMode) {
		cout << "Starting the test" << endl;
		result = test(inputFileName);
	}
	else if (entropyMode) {
		result = calcEntropy(inputFileName, entropyOrder);
	}
	else {
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
