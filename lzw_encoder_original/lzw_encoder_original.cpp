#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#define MAX_ALPHABET_SIZE 256
#define CODE_LENGTH 12
#define MAX_DICTIONARY_SIZE 4096
using namespace std;


string filename = "world192.txt";
string outfilename = "world192.txt.lzw";

char * memblock;

char myLine[100];

// Variables to keep track of the position in the input file
char encoded[20]; // "Buffer" to temporarily store output bytes
int byteptr = 0; // Byte position in buffer
int bitptr = 0; // Bit position in buffer

// The dictionary and appended arrays are used to store the
// dictionary (refer to the lab report for further info)
int dictionary[MAX_DICTIONARY_SIZE];
char appended[MAX_DICTIONARY_SIZE];
int dictsize; // Size of dictionary "so far" as the file is encoded
int posinfile; // Position of next char in input file to be encoded
int filesize; // Original file size

// Function to convert a char (range -128 to 127) into an index
// (range 0-255)
int char2index(char c)
{
	int ind = (int)c;
	if (c < 0)
		ind = 256 + ind;
	return ind;
}

// Function to convert the index of a symbol (range 0-255) to the
// corresponding (signed) char (range -128 to 127)
char index2char(int index)
{
	if (index > 127)
		index = index - 256;
	return (char)index;
}

// Function to add some bits (e.g. the code of a character)
// to the char array <encoded[]> where output bytes are temporarily
// stored before being written to file
void add_bits(unsigned int code, unsigned int length = CODE_LENGTH)
{
	int shift = 8 - bitptr - length;
	if (shift > 0) { // Bits fit in current byte with space left
		encoded[byteptr] += (code << shift);
		bitptr += length;
	}
	else if (shift == 0) { // Bits fit exactly in current byte
		encoded[byteptr] += code;
		byteptr++;
		bitptr = 0;
	}
	else { // Too many bits to fit in current byte
		encoded[byteptr] += (code >> (-shift));
		byteptr++;
		bitptr = 0;
		unsigned int bits_left = -shift; // Number of remaining bits
		// Write the remaining bits by recursively calling add_bits()
		unsigned int bitmask = 1;
		for (unsigned int l = 1; l < bits_left; l++)
			bitmask = (bitmask << 1) + 1;
		add_bits(code&bitmask, bits_left);
	}
}

// Function for matching the upcoming characters in the file against
// the dictionary. Returns index of longest match with the rest of the
// file stored in memblock. The global variable <posinfile> is used
// to know where in the file to start matching, and is updated by this
// function.
int match_string()
{
	// Initialize <longest>, which stores the index of the longest
	// match so far, with the index corresponding to the first
	// upcoming character
	int longest = char2index(memblock[posinfile]);
	posinfile++;
	int k = 256;
	// Step through dictionary and look for longer matches
	while ((k < dictsize) && (posinfile < filesize))
	{
		if ((dictionary[k] == longest) && (appended[k] == memblock[posinfile]))
		{
			// Found match
			longest = k;
			posinfile++;
		}
		k++;
	}
	return longest;
}

int main()
{
	// Initialize dictionary
	for (int i = 0; i < MAX_ALPHABET_SIZE; i++)
	{
		dictionary[i] = -1;
		appended[i] = index2char(i);
	}

	dictsize = MAX_ALPHABET_SIZE;

	ifstream::pos_type insize;
	ifstream::pos_type outsize;

	ifstream infile(filename, ios::in | ios::binary | ios::ate);

	double outsizedouble;

	if (infile.is_open())
	{
		cout << "Opened file..." << endl;
		insize = infile.tellg();
		filesize = (int)insize;

		memblock = new char[filesize];
		infile.seekg(0, ios::beg);
		infile.read(memblock, insize);
		infile.close();

		ofstream fout(outfilename, ios::binary);

		cout << "Opened output filestream... " << endl << endl;

		int matchID;
		posinfile = 0;

		// Encode the file
		while (posinfile < filesize)
		{

			matchID = match_string(); // Find best (partial) match

			// Add code for the match to the output
			add_bits((unsigned int)matchID);
			// Write any finished bytes to file buffer
			if (byteptr > 0)
			{
				fout.write(&encoded[0], byteptr);
				// Move the byte currently being worked on to the front,
				// clear the rest of encoded[] and reset byte pointer
				encoded[0] = encoded[byteptr];
				byteptr = 0;
				for (int l = 1; l < 20; l++)
					encoded[l] = 0;
			}

			if (dictsize < (MAX_DICTIONARY_SIZE - 1))
			{
				// Dictionary not yet full, make new dictionary entry
				// (The biggest code value, MAX_DICTIONARY_SIZE-1, is
				// reserved for the EOF code)
				dictionary[dictsize] = matchID;
				appended[dictsize] = memblock[posinfile];
				dictsize++;
			}

		}

		// Add EOF code
		add_bits(MAX_DICTIONARY_SIZE - 1);

		// Write the last few bytes and close filestream
		fout.write(&encoded[0], byteptr);
		outsizedouble = (double)(fout.tellp());
		fout.close();

		delete[] memblock;

		cout << "Finished decoding." << endl;
		cout << "Compression ratio: " << posinfile / outsizedouble << endl;
		cout << "Data rate: " << outsizedouble * 8 / posinfile;
		cout << " bits per input byte" << endl;
	}
	else {
		cout << "Could not open file" << endl;
	}

	cin.getline(myLine, 100);
}
