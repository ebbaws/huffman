#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <math.h>
using namespace std;

ifstream::pos_type file_size;
char * memblock;

int nuniq = 0; // Number of unique <order+1>-length sequences
char myLine[100];
const int order = 2; // Order (# of characters in memory)
char(*uniqs)[order + 1]; // Array to store all unique sequences
int * stats; // Vector to store # of occurrences for each unique sequence

string filename = "alice29.txt";
string ordinal = "Second";

int main() {

	ifstream file(filename, ios::in | ios::binary | ios::ate);
	if (file.is_open())
	{
		file_size = file.tellg();
		memblock = new char[(int)file_size];
		file.seekg(0, ios::beg);
		file.read(memblock, file_size);
		file.close();

		cout << "The complete file content is in memory. " << endl;

		uniqs = new char[(int)file_size][order + 1];
		stats = new int[(int)file_size];

		// Array to store the latest <order+1> characters
		char last[order + 1];
		int match;
		// Step through the loaded file and count occurrences for
		// every <order+1>-char sequence
		for (int j = order; j < (int)file_size; j++)
		{
			match = -1;
			for (int k = 0; k < (order + 1); k++)
				last[k] = memblock[j - order + k];

			int p = 0;
			// Check if this sequence has been encountered before
			int matchfound;
			while ((p <= nuniq) && (match == -1))
			{
				matchfound = 1;
				for (int l = 0; l < (order + 1); l++)
					matchfound = matchfound && (last[l] == uniqs[p][l]);

				if (matchfound)
				{
					match = p;
				}
				p++;
			}

			if (match == -1)
			{
				// If no match, add this sequence to the list
				for (int p = 0; p < (order + 1); p++)
					uniqs[nuniq][p] = last[p];
				stats[nuniq] = 1;
				// Increment number of unique <order+1>-length sequences
				nuniq++;
			}
			else {
				// If match, increment statistics for this sequence
				stats[match]++;
			}
		}

		delete[] memblock;

		int k = 0;
		char state[order];
		int cstats[256];
		int scnt;
		int totstats;
		double entropy = 0.0;
		double lnm2 = 1 / log(2.0);

		// In the next loop we go through the list of unique
		// <order+1>-length character sequences. In each iteration we
		// look at all sequences that have the first <order> characters
		// in common, compiling statistics about the last character,
		// estimating static and conditional probabilities and
		// estimating the entropy from there.
		while (k < nuniq)
		{
			// Save this iteration's "first <order> characters"
			for (int l = 0; l < order; l++)
				state[l] = uniqs[k][l];

			cstats[0] = stats[k];
			totstats = stats[k];
			scnt = 1;

			int matchfound;

			// <cstats[]> will be used to count the # of occurrences
			// for each character after these first <order> characters
			// and <totstats[]> stores the total # of occurrences
			// of <order+1>-length sequences with these first <order>
			// characters (equal to the sum of all elements in
			// <cstats[]>). <scnt> is the total number of unique such
			// sequences. The counting is done in the following loop
			for (int j = (k + 1); j < nuniq; j++)
			{
				if (stats[j] >= 0)
				{
					matchfound = 1;
					for (int l = 0; l < order; l++)
						matchfound = matchfound && (state[l] == uniqs[j][l]);

					if (matchfound)
					{
						// Found unique sequence that has the right
						// <order> first characters
						cstats[scnt] = stats[j];
						totstats += stats[j];
						// Mark this sequence as "already dealt with"
						stats[j] = -1;
						scnt++;
					}
				}
			}

			// Compute this iteration's contribution to the entropy
			double prob = (double)totstats / ((int)file_size - order);

			double condprob;
			double condent = 0.0;
			for (int j = 0; j < scnt; j++)
			{
				condprob = cstats[j] / double(totstats);
				condent += (-condprob * lnm2*log(condprob));
			}
			// Add contribution to total entropy
			entropy += prob * condent;

			k++;
			while (stats[k] < 0)
			{
				// Step past any sequences that have already been
				// "dealt with"
				k++;
			}
		}

		cout << endl << ordinal << "-order entropy: " << entropy;
		cout << " bits/char" << endl;

		cin.getline(myLine, 100);

		delete[] stats;
		delete[] uniqs;

	}
	else cout << "Unable to open file";
	cin.getline(myLine, 100);

	return 0;
}
