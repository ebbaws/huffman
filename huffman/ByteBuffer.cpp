#include "ByteBuffer.h"
#include <fstream>
#include <iostream>
#include "misc.h"
using namespace std;

ByteBuffer::ByteBuffer()
{
	for (int i = 0; i < bufferSize; i++) bytes[i] = 0;
}

ByteBuffer::~ByteBuffer()
{
}

void ByteBuffer::addBits(unsigned long long code, int length)
{
	int shift = 8 - currentBit - length;
	if (shift > 0) { // Bits fit in current byte with space left
		bytes[currentByte] += static_cast<char>(code << shift);
		currentBit += length;
	}
	else if (shift == 0) { // Bits fit exactly in current byte
		bytes[currentByte] += static_cast<char>(code); 
		currentByte++;
		currentBit = 0;
	}
	else { // Too many bits to fit in current byte
		bytes[currentByte] += static_cast<char>(code >> (-shift)); 
		currentByte++;
		currentBit = 0;
		unsigned int bits_left = -shift; // Number of remaining bits
		// Write the remaining bits by recursively calling addBits()
		unsigned long bitmask = 1;
		for (unsigned int l = 1; l < bits_left; l++)
			bitmask = (bitmask << 1) + 1;
		addBits(code&bitmask, bits_left);
	}
}

void ByteBuffer::writeBytes(std::ofstream& stream,
	bool writeUnfinished)
{
	// Write finished bytes to stream
	stream.write(&bytes[0], currentByte);

	// Move current byte to top, clear rest of buffer
	bytes[0] = bytes[currentByte];
	for (int i = 1; i <= currentByte; i++) bytes[i] = 0;
	currentByte = 0;

	if (writeUnfinished&&(currentBit!=0)) {
		// Also write the incomplete byte at the end
		stream.write(&bytes[0], currentByte);
		currentBit = 0;
	}
}

void ByteBuffer::print() {
	cout << "Current state of byte buffer: " << endl;
	for (int i=0; i <= currentByte; i++) {
		cout << char2index(bytes[i]) << " ";
	}
	cout << endl;
	cout << "Current byte: " << currentByte << ", current bit: " <<
		currentBit << endl;
}
