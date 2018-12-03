#pragma once
#include <string>
using namespace std;

class HuffmanEncoder
{
private:
	string inputFilePath = "";
	string outputFilePath = "";

public:
	HuffmanEncoder();
	~HuffmanEncoder();
	void setInputFile(string path);
	void setOutputFile(string path);
	bool encode();

};

