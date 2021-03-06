#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#define MAX_ALPHABET_SIZE 256
using namespace std;

string filename = "alice29.txt.huff";
string outfilename = "alice29.txt.hdec";
int tobesorted[MAX_ALPHABET_SIZE]; // Values to be sorted by
int indexes[MAX_ALPHABET_SIZE]; // Indexes to be sorted
unsigned int huffmancodes[MAX_ALPHABET_SIZE][2]; // Code table
char myLine[100];
unsigned int eofcode[2];

// Variables for the encoding step
int byteptr = 0;
int bitptr = 0;
char nextbytes[5];
int minlength, maxlength;

// Sorting function, based on Quicksort
void sortfunc(int start, int end)
{
	if (start < end)
	{
		int pivotind = indexes[start];
		int pivotval = tobesorted[pivotind];
		int pivotpos = start;
		int end2 = end;
		int compareind;
		int compareval;
		for (int j = 0; j < (end - start); j++)
		{
			compareind = indexes[pivotpos + 1];
			compareval = tobesorted[compareind];
			if (compareval > pivotval)
			{
				indexes[pivotpos] = compareind;
				pivotpos++;
			}
			else {
				indexes[pivotpos + 1] = indexes[end2];
				indexes[end2] = compareind;
				end2--;
			}
		}
		indexes[pivotpos] = pivotind;
		sortfunc(start, pivotpos - 1);
		sortfunc(pivotpos + 1, end);
		return;

	}
	else {
		return;
	}

}

// Function to convert the index of a symbol (range 0-255) to the
// corresponding (signed) char (range -128 to 127)
char index2char(int index)
{
	if (index > 127)
		index = index - 256;
	return (char)index;
}

// Function to convert a char (range -128 to 127) into an index
// (range 0-255)
int char2index(char c)
{
	int ind = (int)c;
	if (c < 0)
		ind = 256 + ind;
	return ind;
}

// Match a code against the code table
int match_code(unsigned int code, unsigned int length)
{
	int match = -1;
	if ((code == eofcode[0]) && (length == eofcode[1]))
		match = 256;
	else
	{
		int j = 0;
		// Increment <j> up to the first index that represents a code
		// with length <length> or more
		while (huffmancodes[indexes[j]][1] < length)
			j++;
		// If there is at least one code with length <length>
		// then step through those codes and compare to <code>
		if (huffmancodes[indexes[j]][1] == length)
		{
			int thiscode;
			int iterating = 1;
			while (iterating)
			{
				thiscode = huffmancodes[indexes[j]][0];
				if (thiscode == code)
					match = indexes[j];
				j++;
				if (j > 255)
				{
					iterating = 0;
				}
				else {
					if (huffmancodes[indexes[j]][1] != length)
						iterating = 0;
					if (match != -1)
						iterating = 0;
				}
			}
		}
	}
	return match;
}

// Function to determine which code the upcoming <length> bits make 
unsigned int extract_code(unsigned int length)
{
	int code = 0;
	// Make bitmask to "cut out" upcoming bit
	char bitmask = index2char(128 >> bitptr);
	int bytenr = 0; // Byte "offset" from current byte pointer
	char currentbyte = nextbytes[bytenr];
	// Step through upcoming <length> bits
	for (int i = bitptr; i < (bitptr + (int)length); i++)
	{
		code = code * 2; // Left shift code
		if (currentbyte&bitmask)
			code++; // Add 1 if bit value is 1
		if (bitmask == 1)
		{
			// Reached end of byte, make some updates
			bytenr++;
			currentbyte = nextbytes[bytenr];
			bitmask = index2char(128);
		}
		else {
			// Right shift bit mask to make it "cut out" next bit
			bitmask = index2char(char2index(bitmask) >> 1);
		}
	}
	return code;
}

// Function to find the next code.
int match_upcoming()
{
	unsigned int matchlength = minlength;
	int match = -1;
	int matchcode;
	// Match upcoming bits against code table for different lengths
	while (((int)matchlength <= maxlength) && (match == -1))
	{
		matchcode = extract_code(matchlength);
		match = match_code(matchcode, matchlength);
		matchlength++;
	}
	// Update byte and bit pointers
	int ptrupdate = bitptr + matchlength - 1;
	bitptr = ptrupdate % 8;
	byteptr = byteptr + ptrupdate / 8;
	return match;
}

int main()
{
	ifstream::pos_type size;
	char * memblock;
	int split, alphsize;
	ifstream file(filename, ios::in | ios::binary | ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		memblock = new char[(int)size];
		file.seekg(0, ios::beg);
		file.read(memblock, size);
		file.close();

		cout << "File loaded..." << endl;

		alphsize = 0;
		for (int i = 0; i < MAX_ALPHABET_SIZE; i++)
		{
			huffmancodes[i][1] = char2index(memblock[i]);
			if (huffmancodes[i][1] != 0)
				alphsize++;
		}

		// Save index of the node that was split to make the
		// EOF node, which was sent as side info
		split = char2index(memblock[256]);

		// Reconstruct tree from codelengths.
		// First sort the code lengths in increasing order
		// by using "sortfunc" again
		for (int j = 0; j < MAX_ALPHABET_SIZE; j++)
		{
			indexes[j] = j;
			tobesorted[j] = 256 - huffmancodes[j][1];
		}
		sortfunc(0, MAX_ALPHABET_SIZE - 1);

		// Make new codewords from the given codeword lengths
		// using the algorithm from TSBK08 Lecture 3 (2012)
		int ptr = MAX_ALPHABET_SIZE - alphsize;
		int code = 0;
		huffmancodes[ptr][0] = code;

		// Save min and max code lengths for later use
		minlength = huffmancodes[indexes[ptr]][1];
		maxlength = huffmancodes[indexes[255]][1] + 1;
		// (Maxlength was incremented because the tree will be extended
		// with the EOF code)

		int thislength = huffmancodes[indexes[ptr]][1];
		int oldlength;

		ptr++;

		for (ptr; ptr < MAX_ALPHABET_SIZE; ptr++)
		{
			oldlength = thislength;
			thislength = huffmancodes[indexes[ptr]][1];
			code = code + 1;
			code = code * (int)pow(2.0, (thislength - oldlength));
			huffmancodes[indexes[ptr]][0] = code;
		}

		// Modify the tree slightly to include an EOF code
		// by splitting the "least likely" node
		huffmancodes[split][0] = 2 * huffmancodes[split][0];
		huffmancodes[split][1]++;
		eofcode[0] = huffmancodes[split][0] + 1;
		eofcode[1] = huffmancodes[split][1];
		if (eofcode[1] > 32)
			cout << "EOF code length exceeding 32 bits!" << endl;

		// Start decoding the encoded sequence

		// First set byte and bit pointer
		byteptr = 257;
		bitptr = 0;

		// Initialize nextbytes[], a global variable which should 
		// always hold the next few bytes to be decoded
		int tmpbyteptr;
		for (int i = 0; i < 5; i++)
		{
			tmpbyteptr = byteptr + i;
			if (tmpbyteptr < (int)size)
				nextbytes[i] = memblock[tmpbyteptr];
		}

		// Open the output file
		ofstream fout(outfilename, ios::binary);

		cout << "Opened output filestream... " << endl;

		int matchindex;
		char matchchar;
		int keepgoing = 1;
		while (keepgoing)
		{
			matchindex = match_upcoming(); // Look for match
			if (matchindex == 256) // EOF found
			{
				cout << "Finished decoding. " << endl;
				keepgoing = 0;
			}
			else if ((matchindex > 256) || (matchindex < 0))
			{
				// Invalid code received
				cout << "No code found. Stopped decoding." << endl;
				keepgoing = 0;
			}
			else { // Code found
				matchchar = index2char(matchindex);
				fout.write(&matchchar, 1);
				// Update nextbytes[] with upcoming bytes
				for (int i = 0; i < 5; i++)
				{
					tmpbyteptr = byteptr + i;
					if (tmpbyteptr < (int)size + 1)
						nextbytes[i] = memblock[tmpbyteptr];
				}
				if (byteptr > (int)size)
				{
					// Came to end of input file without finding EOF
					cout << "Couldn't find EOF code.";
					cout << " Stopped decoding." << endl;
					keepgoing = 0;
				}
			}


		}

		fout.close();

		delete[] memblock;
	}
	else cout << "Unable to open file.";

	cin.getline(myLine, 100);
}
