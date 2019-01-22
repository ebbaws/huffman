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
