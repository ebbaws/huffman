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
//using namespace std;

bool encode(std::string & inputFilePath, std::string & outputFilePath) {
	HuffmanEncoder encoder;
	encoder.setInputFile(inputFilePath);
	encoder.setOutputFile(outputFilePath);
	
	return encoder.encode();
}

bool decode(std::string & inputFilePath, std::string & outputFilePath) {
	
	Decoder decoder;
	decoder.setInputFile(inputFilePath);
	decoder.setOutputFile(outputFilePath);

	return decoder.decode();

}

bool calcEntropy(std::string & inputFilePath, int order) {
	EntropyCalculator calculator;
	double entropy;

	if (order > 0) {
		entropy = calculator.calcHigherOrderEntropy(inputFilePath, order);
	}
	else {
		entropy = calculator.calcMemorylessEntropy(inputFilePath);
	}
	
	if (entropy < 0.0) {
		std::cout << "Entropy calculation failed" << std::endl;
		return false;
	}
	else {
		std::cout << "Entropy: " << entropy << std::endl;
		return true;
	}

}

bool test(std::string & inputFilePath) {
	std::string encodedFilePath = "encoded.huff";
	std::string decodedFilePath = "decoded.huff";
	if (encode(inputFilePath, encodedFilePath)&&
		decode(encodedFilePath, decodedFilePath)) {

		return (compareFiles(inputFilePath, decodedFilePath));

	}
	else {
		std::cout << "Couldn't finish test" << std::endl;
		return false;
	}

}


void printUsageInstructions() {
	using std::cout;
	using std::endl;
	cout << "Please specify an input file name" << endl;
	cout << "Default mode: encode" << endl;
	cout << "For decode mode, write -decode" << endl;
	cout << "For entropy calculation mode, write -entropy" << endl;
	cout << "Debug options: -test, -testsort" << endl;
	cout << "Output file name (for decode or encode mode)" <<
		" can also be specified after input file name" << endl;
}

int main (int argc, char *argv[])  {
	using std::string;

	string inputFileName = "";
	string outputFileName = "output.huff";

	enum Mode {
		MODE_ENCODE,
		MODE_DECODE,
		MODE_ENTROPY,
		MODE_TEST,
		MODE_TEST_SORT
	};
	
	// Parse arguments
	Mode mode(MODE_ENCODE);
	int entropyOrder = 0;
	bool waitingForEntropyOrder = false;
	bool modeSpecified = false;

	using std::cout;
	using std::endl;

	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			string argString(argv[i], strlen(argv[i]));

			if (waitingForEntropyOrder) {
				// Determine if this argument is a number.
				// If so, extract it and assign it to entropyOrder
				int order = stringToPositiveInt(argString);
				waitingForEntropyOrder = false;
				if (order >= 0) {
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
					mode = MODE_DECODE;
				} else if (!argString.compare(testString)) {
					mode = MODE_TEST;
				}
				else if (!argString.compare(testSortString)) {
					mode = MODE_TEST_SORT;
				}
				else if (!argString.compare(entropyString)) {
					mode = MODE_ENTROPY;
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

	if (mode != MODE_TEST_SORT && inputFileName == "") {
		printUsageInstructions();
		return -1;
	}

	// Do what was requested

	bool result = false;

	switch (mode) {
	case MODE_ENCODE:
		cout << "Attempting to encode file " <<
			inputFileName << endl;
		result = encode(inputFileName, outputFileName);
		break;

	case MODE_DECODE:
		cout << "Attempting to decode file " <<
			inputFileName << endl;
		result = decode(inputFileName, outputFileName);
		break;

	case MODE_ENTROPY:
		cout << "Attempting to calculate order-" << entropyOrder <<
			" entropy for " << inputFileName << endl;
		result = calcEntropy(inputFileName, entropyOrder);
		break;

	case MODE_TEST:
		cout << "Testing encoding and decoding on " <<
			inputFileName << endl;
		result = test(inputFileName);
		break;
	
	case MODE_TEST_SORT:
		cout << "Testing sorting algorithm" << endl;
		result = true;
		testSort();
		break;

	}

	if (result && (mode == MODE_ENCODE || mode == MODE_DECODE)) {
		cout << "Result was written to file " <<
			outputFileName << endl;
	}

	return result ? 0 : -1;

}
