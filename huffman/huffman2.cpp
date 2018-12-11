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

string infilename = "C:\\learn\\cantrbry\\asyoulik.txt";
string outfilename = "output.huff";

//char myLine[100];


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

void testDecode() {
	encode(infilename, outfilename);

	string decoderOutputFile = "asyoulik_decoded.txt";

	decode(outfilename, decoderOutputFile);
}

void printUsageInstructions() {
	cout << "Please specify an input file name" << endl;
	cout << "For decode mode, write -decode" << endl;
	cout << "Output file name can also be specified after input file name" << endl;
}

int main (int argc, char *argv[])  {
	string inputFileName = "";
	string outputFileName = "output.huff";

	bool decodeMode = false;

	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			string argString(argv[i], strlen(argv[i]));

			if (argv[i][0] == '-') {
				string decodeString = "-decode";
				if (!argString.compare(decodeString)) {
					decodeMode = true;
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

	if (inputFileName == "") {
		printUsageInstructions();
	}
	else {
		cout << "Input file name: " << inputFileName << endl;
		cout << "Output file name: " << outputFileName << endl;
		if (decodeMode) {
			cout << "Decode mode" << endl;
		}
		else {
			cout << "Encode mode" << endl;
		}
	}

}
