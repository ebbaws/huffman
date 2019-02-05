#ifndef HUFFMANENCODER_H
#define HUFFMANENCODER_H

#include <string>
//using namespace std;

class HuffmanEncoder
{
private:
	std::string inputFilePath = "";
	std::string outputFilePath = "";

public:
	HuffmanEncoder();
	~HuffmanEncoder();
	void setInputFile(std::string path);
	void setOutputFile(std::string path);
	bool encode();

};

#endif