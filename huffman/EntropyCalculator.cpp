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

double EntropyCalculator::calcHigherOrderEntropy(std::string filePath,
	int order)
{

	ifstream::pos_type size;
	char latestByte;
	int sequenceLength = order + 1;
	int numUniqueSequences = 0;
	int numNonUniqueSequences;

	char * uniqs; // To store unique char sequences
	int * sequenceStats; // To store # of occurrences
	char * latest; // Buffer for latest characters read

	ifstream file(filePath, ios::in | ios::binary | ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		file.seekg(0, ios::beg);
		cout << "File loaded." << endl;
		
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

		numNonUniqueSequences = static_cast<int>(size) - order;

		cout << "Gathering sequence statistics (may take some time) ";

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

			// Search for matches among our unique sequences so far
			bool matchFound = false;
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

			if (static_cast<int>(file.tellg()) % 1500 == 0) {
				cout << '.';
			}

		}
		cout << " done" << endl;
		file.close();

	}
	else {
		cout << "Unable to open file." << endl;
		return -1.0;
	}

	
	cout << "Found " << numUniqueSequences << " unique sequences of length "
		<< sequenceLength << endl;

	cout << "Computing entropy" << endl;
	
	int k = 0;
	char * state = new char[order];
	int charStats[256];
	int seqCount;
	int stateStats;
	double entropy = 0.0;
	double logConverter = 1 / log(2.0);

	// In the next loop we go through the list of unique
	// <order+1>-length character sequences. In each iteration we
	// look at all sequences that have the first <order> characters
	// in common, compiling statistics about the last character,
	// estimating static and conditional probabilities and
	// estimating the entropy from there.
	
	while (k < numUniqueSequences) {
		// Save this iteration's state (first <order> characters)
		for (int l = 0; l < order; l++)
			state[l] = uniqs[k*sequenceLength+l];

		seqCount = 1;
		charStats[0] = sequenceStats[k];
		stateStats = sequenceStats[k];		

		// Compile conditional character statistics for this state
		for (int j = (k + 1); j < numUniqueSequences; j++) {
			if (sequenceStats[j] > 0) {
				bool matchFound = true;
				for (int i = 0; i < order; i++) {
					matchFound = matchFound &&
						(uniqs[j*sequenceLength + i] == state[i]);
				}

				if (matchFound) {
					charStats[seqCount] = sequenceStats[j];
					stateStats = stateStats + sequenceStats[j];
					seqCount++;
					sequenceStats[j] = -1; // Mark sequence as "dealt with"
				}
			}
		}

		// Compute this state's entropy contribution
		double stateProb = static_cast<double>(stateStats) / numNonUniqueSequences; //??
		double condEntropy = 0.0;

		for (int i = 0; i < seqCount; i++) {
			double condProb = static_cast<double>(charStats[i]) / stateStats;
			condEntropy -= condProb * logConverter * log(condProb);
		}

		entropy += stateProb * condEntropy; // Add to total entropy

		k++;
		while (sequenceStats[k] < 0)
		{
			// Step past any sequences that have already been matched
			k++;
		}
		//cout << '.';
	}
	cout << endl;

	delete[] uniqs;
	delete[] sequenceStats;
	delete[] latest;

	return entropy;
}
