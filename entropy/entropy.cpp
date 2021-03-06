#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#define MAX_ALPHABET_SIZE 256
using namespace std;

string filename = "C:\\learn\\cantrbry\\fields.c";

char myLine[100];

// Function to convert a char (range -128 to 127) into an index
// (range 0-255)
int char2index(char c)
{
	int ind = (int)c;
	if (c < 0)
		ind = 256 + ind;
	return ind;
}

int main()
{

	ifstream::pos_type size;
	char * memblock;

	double probs[256]; // Probabilities for each character

	ifstream file(filename, ios::in | ios::binary | ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		memblock = new char[(int)size];
		file.seekg(0, ios::beg);
		file.read(memblock, size);

		file.close();

		cout << "The complete file content is in memory";

		// Initialize the probability array with zeroes
		for (int j = 0; j < 256; j++)
		{
			probs[j] = 0.0;
		}

		// Count occurrences of different characters
		for (int j = 0; j < size; j++)
		{
			probs[char2index(memblock[j])]++;
		}
		delete[] memblock;
	}
	else cout << "Unable to open file";

	// Replace the numbers of occurrences by probabilities
	// by dividing by the total number of characters
	double probfac = 1 / (double)size;
	for (int j = 0; j < 256; j++)
	{
		probs[j] = probs[j] * probfac;
		cout << probs[j] << " ";
	}
	cout << endl;

	double entropy = 0.0;
	double lnm2 = 1 / log(2.0);

	// Compute the entropy from the probabilities
	for (int j = 0; j < 256; j++)
		if (probs[j] != 0.0)
		{
			entropy += (-probs[j] * (log(probs[j])*lnm2));
		}

	cout << endl << "Zero-order entropy: " << entropy << endl;

	cin.getline(myLine, 100);

}
