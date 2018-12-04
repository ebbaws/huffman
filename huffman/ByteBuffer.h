#ifndef BYTEBUFFER_H
#define BYTEBUFFER_H

#include <fstream>

class ByteBuffer
{
private:
	static const int bufferSize = 20; // should be enough
	char bytes[bufferSize];
	int currentBit = 0;
	int currentByte = 0;

public:
	ByteBuffer();
	~ByteBuffer();
	void addBits(unsigned long long code, int length);
	void writeBytes(std::ofstream& stream,
		bool writeUnfinished = false); 
	void print();
};

#endif

