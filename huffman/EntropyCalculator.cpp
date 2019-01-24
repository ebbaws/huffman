#include "EntropyCalculator.h"
#include <fstream>
#include <iostream>
#include "misc.h"
using namespace std;

EntropyCalculator::EntropyCalculator()
{
}


EntropyCalculator::~EntropyCalculator()
{
}

double EntropyCalculator::calcMemorylessEntropy(std::string filePath)
{
	int charStats[maxAlphabetSize];

	ifstream::pos_type size;
	char latestByte;

	for (int j = 0; j < maxAlphabetSize; j++)
		charStats[j] = 0; // Initialize statistics

	ifstream file(filePath, ios::in | ios::binary | ios::ate);
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
		return -1.0;
	}

	double normalizer = 1 / (double)size;

	double probabilities[maxAlphabetSize];
	for (int i = 0; i < maxAlphabetSize; i++) {
		probabilities[i] = normalizer * charStats[i];
	}

	double entropy = 0.0;
	double logConverter = 1 / log(2.0);

	for (int i = 0; i < maxAlphabetSize; i++)
	{
		if (charStats[i] != 0)
		{
			entropy += (-probabilities[i] * (log(probabilities[i])*logConverter));
		}
	}

	return entropy;
}


void printWithoutNewlineEtc(char c) {
	if (c == '\n') {
		cout << "\\n";
	}
	else if (c == '\t') {
		cout << "\\t";
	}
	else {
		cout << c;
	}
}

double EntropyCalculator::calcHigherOrderEntropy(std::string filePath,
	int order)
{

	ifstream::pos_type size;
	char latestByte;
	int sequenceLength = order + 1;
	int numUniqueSequences = 0;

	char * uniqs; // To store unique char sequences
	int * sequenceStats; // To store # of occurrences
	char * latest; // Buffer for latest characters read

	ifstream file(filePath, ios::in | ios::binary | ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		file.seekg(0, ios::beg);
		cout << "File loaded." << endl;

		cout << "Gathering statistics..." << endl;

		try {
			uniqs = new char[sequenceLength*(int)size];
			sequenceStats = new int[(int)size];
			latest = new char[sequenceLength];
		}
		catch(exception &e) {
			cout << "Something went wrong: " << e.what() << endl;
			file.close();
			return -1.0;
		}

		// Read the first few characters
		for (int i = 1; i < sequenceLength; i++) {
			if (file.get(latestByte)) {
				latest[i] = latestByte;
			}
		}

		while (file.get(latestByte)) {
		   
			// Move previous bytes back in the buffer
			// and add the new one
			for (int i = 1; i < sequenceLength; i++) {
				latest[i-1] = latest[i];
			}
			latest[sequenceLength - 1] = latestByte;

			if (numUniqueSequences < 100) {
				for (int j = 0; j < sequenceLength; j++) {
					printWithoutNewlineEtc(latest[j]);
				}
				cout << endl;
			}
			

			// Search for matches among our unique sequences so far
			int matchFound = false;
			int charIdx = 0;
			for (int i = 0; i < numUniqueSequences; i++) {
				// Compare the latest sequence to this one
				bool potentialMatch = true;
				for (int j = 0; j < sequenceLength; j++) {
					if (potentialMatch) {
						bool charMatch = latest[j] == uniqs[charIdx];
						potentialMatch = potentialMatch && charMatch;
					}
					charIdx++;
				}
				if (potentialMatch) {
					matchFound = true;
					sequenceStats[i]++;
					break;
				}

			}

			if (!matchFound) {
				// Make new unique-sequence entry
				sequenceStats[numUniqueSequences] = 1;
				for (int j = 0; j < sequenceLength; j++) {
					uniqs[charIdx] = latest[j];
					charIdx++;
				}
				numUniqueSequences++;

			}
		}

		file.close();

	}
	else {
		cout << "Unable to open file." << endl;
		return -1.0;
	}

	
	cout << "Found " << numUniqueSequences << " unique sequences" << endl;

	for (int i = 0; i < numUniqueSequences; i++) {
		for (int j = i*sequenceLength; j < (i+1)*sequenceLength; j++) {
			printWithoutNewlineEtc(uniqs[j]);
		}
		cout << " (" << sequenceStats[i] << "); ";
	}
	cout << endl;

	double entropy = 0.0;

	
	return entropy;
}
