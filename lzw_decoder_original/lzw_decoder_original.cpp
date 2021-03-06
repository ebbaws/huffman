#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#define MAX_ALPHABET_SIZE 256
#define CODE_LENGTH 12
#define MAX_DICTIONARY_SIZE 4096
using namespace std;

string infilename = "world192.txt.lzw";
string outfilename = "world192.txt.lzwd";

char * memblock;
char myLine[100];

// Byte and bit pointers for the input
int byteptr = 0;
int bitptr = 0;

// The <dictionary[]> and <appended[]> arrays are used to store the
// dictionary (refer to the lab report for further info)
int dictionary[MAX_DICTIONARY_SIZE];
char appended[MAX_DICTIONARY_SIZE];
// Array for temporarily storing upcoming output characters:
char entrychars[MAX_DICTIONARY_SIZE];
int dictsize; // Size of dictionary "so far" as the file is encoded
int filesize; // Output file size
int matchlength; // Length of latest dictionary entry found

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

// Function to get the characters in a dictionary entry represented by
// <index>. Puts those characters in global char array <entrychars[]>
// and updates <matchlength> with length of entry.
void get_entry(int index)
{
	int chrcnt = 0; // Chars so far in entry
	if (index < dictsize) // Dictionary entry already exists
	{
		int thisindex = index;

		while (thisindex >= 0)
		{
			entrychars[chrcnt] = appended[thisindex];
			thisindex = dictionary[thisindex];
			chrcnt++;
		}
		// Reverse order of entrychars so that the first character
		// comes first.
		char replaced;
		for (int i = 0; i < (chrcnt / 2); i++)
		{
			replaced = entrychars[chrcnt - i - 1];
			entrychars[chrcnt - i - 1] = entrychars[i];
			entrychars[i] = replaced;
		}
	}
	else if (index == dictsize) {
		// Dictionary entry is the one being worked on right now
		chrcnt = matchlength;
		entrychars[matchlength] = entrychars[0];
		chrcnt++;
	}
	else {
		chrcnt = -1;
		cout << "Bad code! " << endl;
	}
	matchlength = chrcnt;
}

// Function to determine which code the upcoming
// <CODE_LENGTH> bits make 
unsigned int extract_code()
{
	int code = 0;
	// Make a bitmask that "cuts out" one bit
	char bitmask = index2char(128 >> bitptr);
	int bytenr = 0;
	char currentbyte = memblock[byteptr + bytenr];
	// Extract code one bit at a time
	for (int i = bitptr; i < (bitptr + CODE_LENGTH); i++)
	{
		code = code * 2; // Left shift the code
		if (currentbyte&bitmask)
			code++; // Add 1 to code if received bit is 1
		if (bitmask == 1)
		{
			// Reached end of byte, make some updates
			bytenr++;
			currentbyte = memblock[byteptr + bytenr];
			bitmask = index2char(128);
		}
		else {
			// Right shift bitmask to cut out next bit
			bitmask = index2char(char2index(bitmask) >> 1);
		}
	}
	return code;
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

	// Open input file stream
	ifstream::pos_type insize;
	ifstream infile(infilename, ios::in | ios::binary | ios::ate);
	int filesize;

	if (infile.is_open())
	{
		insize = infile.tellg();
		filesize = (int)insize;

		memblock = new char[filesize];// Put input file in <memblock[]>
		infile.seekg(0, ios::beg);
		infile.read(memblock, insize);
		infile.close();
		ofstream fout(outfilename, ios::binary);

		cout << "Opened output filestream." << endl;

		byteptr = 0;
		int keepgoing = 1;

		// Decode the first code in the file
		unsigned int matchID = extract_code();
		get_entry(matchID);
		// Start making the first dictionary entry by adding a reference
		// to an earlier entry with index <matchID>
		dictionary[dictsize] = matchID;

		// Update bit and byte pointers
		int something = bitptr + CODE_LENGTH;
		bitptr = something % 8;
		byteptr = byteptr + something / 8;
		something = bitptr + CODE_LENGTH;
		int nxtbyteptr = byteptr + something / 8;
		int nxtbitptr = something % 8;

		//Write decoded chars to file
		fout.write(&entrychars[0], matchlength);

		// Decode rest of file
		while (keepgoing)
		{
			matchID = extract_code();
			if ((matchID < 0) || (matchID >= MAX_DICTIONARY_SIZE))
				// Invalid code
				keepgoing = 0;
			else if (matchID == (MAX_DICTIONARY_SIZE - 1)) {
				cout << "End-of-file symbol found. " << endl;
				keepgoing = 0;
			}
			else // The code found corresponds to a dictionary entry
			{
				get_entry(matchID);

				if (dictsize < (MAX_DICTIONARY_SIZE - 1))
				{
					// Dictionary not yet full, make new dictionary entry
					appended[dictsize] = entrychars[0];
					dictsize++;
					if (dictsize < MAX_DICTIONARY_SIZE)
						dictionary[dictsize] = matchID;

				}

				// Update bit and byte pointers
				bitptr = nxtbitptr;
				byteptr = nxtbyteptr;
				something = bitptr + CODE_LENGTH;
				nxtbyteptr = byteptr + something / 8;
				nxtbitptr = something % 8;

				// Write decoded chars to file
				fout.write(&entrychars[0], matchlength);

				if ((nxtbyteptr > filesize) && (keepgoing > 0))
				{
					cout << "Stepped past end of file ";
					cout << "without finding EOF symbol. " << endl;
					keepgoing = 0;
				}
			}
		}

		int outsizeint = (int)(fout.tellp());
		fout.close();

		delete[] memblock;

		cout << endl << "Dictionary size: " << dictsize << endl;
		cout << "Characters decoded: " << outsizeint << endl;
	}
	else {
		cout << "Could not open file" << endl;
	}

	cin.getline(myLine, 100);

}
