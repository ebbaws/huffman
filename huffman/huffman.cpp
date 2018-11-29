#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include "header.h"
#define MAX_TREE_SIZE 512
using namespace std;

string infilename = "C:\\learn\\cantrbry\\asyoulik.txt";
string outfilename = "asyoulik.txt.huff";

const int maxAlphabetSize = 256;

int huffman_tree[MAX_TREE_SIZE][3]; // Tree array
unsigned int huffmancodes[maxAlphabetSize][2]; // Code table
char myLine[100];
unsigned int eofcode[2]; // Array to store end-of-file code

char encoded[20]; // Buffer to temporarily store output bytes
int byteptr = 0; // Byte position in buffer
int bitptr = 0; // Bit position in buffer

// Function that "explores" a node in the Huffman tree,
// used recursively to traverse the tree and construct a code table
void explore_node(int nodeID, unsigned int code, unsigned int length)
{
	int leftID = huffman_tree[nodeID][0];
	int rightID = huffman_tree[nodeID][1];
	if ((leftID == -1) && (rightID == -1))
	{
		// Node is leaf
		huffmancodes[nodeID][0] = code;
		huffmancodes[nodeID][1] = length;
		if (length > 32)
			cout << "Code length exceeding 32 bits!" << endl;
	}
	else {
		// Explore children
		explore_node(leftID, code * 2, length + 1);
		explore_node(rightID, code * 2 + 1, length + 1);
	}
	return;
}

// Function to add some bits (e.g. the code of a character)
// to the char array <encoded[]> where output bytes are temporarily
// stored before being written to file
// (move this to separate file?)
void add_bits(unsigned int code, unsigned int length)
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

int main()
{
	int charStats[maxAlphabetSize];
	int indexes[maxAlphabetSize];

	ifstream::pos_type size;
	char * memblock;

	for (int j = 0; j < maxAlphabetSize; j++)
		charStats[j] = 0; // Initialize statistics

	// Open the input file to gather statistics
	ifstream file(infilename, ios::in | ios::binary | ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		memblock = new char[(unsigned int)size];
		file.seekg(0, ios::beg);
		file.read(memblock, size);
		file.close();

		cout << "File loaded." << endl;

		cout << "Gathering statistics..." << endl;

		//Count number of occurrences for every symbol
		for (int j = 0; j < size; j++)
		{
			charStats[(unsigned char)memblock[j]]++;
		}

		delete[] memblock;
	}
	else {
		cout << "Unable to open file." << endl;
		return -1;
	}

	cout << "Building code tree..." << endl;

	mySort(charStats, indexes, maxAlphabetSize);

	int alphsize = maxAlphabetSize;

	// Determine number of non-zero-occurrence characters
	// (only these characters will be included in tree)
	while (charStats[indexes[alphsize - 1]] == 0)
	{
		indexes[alphsize - 1] = -1;
		alphsize--;
	}

	// Initialize the Huffman tree with leaf nodes representing
	// the letters (zero-occurrence letters will be included here,
	// but will not be linked in the tree)
	int treesize = maxAlphabetSize;
	for (int j = 0; j < treesize; j++)
	{
		huffman_tree[j][0] = -1;
		huffman_tree[j][1] = -1;
		huffman_tree[j][2] = charStats[j];
	}

	int end = alphsize - 1;
	int keepgoing = 1;

	// Build the Huffman tree
	while (keepgoing)
	{
		if (treesize < MAX_TREE_SIZE)
		{
			// Create new node
			huffman_tree[treesize][0] = indexes[end - 1];
			huffman_tree[treesize][1] = indexes[end];
			huffman_tree[treesize][2] = huffman_tree[indexes[end - 1]][2]
				+ huffman_tree[indexes[end]][2];

			// Replace the two last nodes in indexes with the new one
			indexes[end] = -1;
			indexes[end - 1] = treesize;
			end--;

			// Step through indexes vector and bubble the new node to the left
			int indexptr = end - 1;
			while ((indexptr >= 0) && (huffman_tree[treesize][2] > huffman_tree[indexes[indexptr]][2]))
			{
				indexes[indexptr + 1] = indexes[indexptr];
				indexes[indexptr] = treesize;
				indexptr--;
			}

			treesize++;

			if (end < 1)
			{
				keepgoing = 0;
			}

		}
		else {
			cout << "Max tree size exceeded (unexpected error)!" << endl;
			return -1;
		}
	}

	//  Construct a table with all the Huffman codes
	//    by traversing the tree.
	for (int j = 0; j < maxAlphabetSize; j++)
	{
		huffmancodes[j][0] = 0;
		huffmancodes[j][1] = 0;
	}
	explore_node(treesize - 1, 0, 0);

	// Modify the tree to a "standard" appearance with the same code
	// lengths so that we will only need the code lengths as side info.

	// OMG DO SOMETHING ABOUT THIS IT's so dumb i cant even
	// First sort the code lengths in increasing order
	// by using sortfunc() again
	for (int j = 0; j < maxAlphabetSize; j++)
	{
		indexes[j] = j;
		// Set values in <charStats[]> to values that decrease
		// with increasing code lengths, so that <indexes[]> can be
		// sorted in ascending order w.r.t. code length
		charStats[j] = 256 - huffmancodes[j][1];
	}

	mySort(charStats, indexes, maxAlphabetSize);

	// Make new codewords from the given codeword lengths
	// using the algorithm from TSBK08 Lecture 3 (2012)
	int ptr = maxAlphabetSize - alphsize;
	int code = 0;
	huffmancodes[indexes[ptr]][0] = code;

	int thislength = huffmancodes[indexes[ptr]][1];
	int oldlength;

	ptr++;

	for (ptr; ptr < maxAlphabetSize; ptr++)
	{
		oldlength = thislength;
		thislength = huffmancodes[indexes[ptr]][1];
		code = code + 1;
		code = code * (int)pow(2.0, (thislength - oldlength));
		huffmancodes[indexes[ptr]][0] = code;
	}

	// Save index of least likely character for later use
	int ll = indexes[255];

	// Modify the tree slightly to include an EOF code
	// by splitting the "least likely" node
	huffmancodes[ll][0] = 2 * huffmancodes[ll][0];
	huffmancodes[ll][1]++;
	eofcode[0] = huffmancodes[ll][0] + 1;
	eofcode[1] = huffmancodes[ll][1];
	if (eofcode[1] > 32)
		cout << "EOF code length exceeding 32 bits!" << endl;

	double insize, outsize;

	// Start encoding. First open input file again
	file.open(infilename, ios::in | ios::binary | ios::ate);
	unsigned int lengthnow, codenow;
	if (file.is_open())
	{
		size = file.tellg();
		insize = (double)size;
		memblock = new char[(int)size];
		file.seekg(0, ios::beg);
		file.read(memblock, size);
		file.close();

		cout << "Number of chars in original file: " << (int)size << endl;

		ofstream fout(outfilename, ios::binary);
		cout << "Writing to file... " << endl;

		// Write side info (code lengths) about the tree to file
		for (int j = 0; j < maxAlphabetSize; j++)
		{
			if (j != ll)
				encoded[0] = index2char(huffmancodes[j][1]);
			else
				encoded[0] = index2char(huffmancodes[j][1] - 1);
			fout.write(&encoded[0], 1);
		}

		encoded[0] = index2char(ll);
		fout.write(&encoded[0], 1);
		encoded[0] = 0;

		// Encode and write to file
		for (int j = 0; j < size; j++)
		{
			codenow = huffmancodes[char2index(memblock[j])][0];
			lengthnow = huffmancodes[char2index(memblock[j])][1];
			add_bits(codenow, lengthnow);
			// Write any finished bytes to file buffer
			if (byteptr > 0)
			{
				fout.write(&encoded[0], byteptr);
				// Move the byte currently being worked on to the front,
				// clear the rest of encoded[] and reset byte pointer
				encoded[0] = encoded[byteptr];
				byteptr = 0;
				for (int k = 1; k < 20; k++)
					encoded[k] = 0;
			}

		}

		// Add EOF code
		add_bits(eofcode[0], eofcode[1]);
		if (byteptr > 0)
		{
			fout.write(&encoded[0], byteptr);
			// Move the byte currently being worked on to the front,
			// clear the rest of encoded and reset byte pointer

			encoded[0] = encoded[byteptr];
			byteptr = 0;
			for (int k = 1; k < 20; k++)
				encoded[k] = 0;

		}

		// Write the last (unfinished) byte and close filestream
		fout.write(&encoded[0], byteptr);
		outsize = (double)(fout.tellp());
		fout.close();

		delete[] memblock;
	}
	else cout << "Unable to open file";

	cout << endl << "Finished writing to file. " << endl;
	cout << "Bits per character (full bytes): " << outsize * 8 / insize << endl;
	cout << "Compression ratio: " << insize / outsize << endl;

	cin.getline(myLine, 100);

}
