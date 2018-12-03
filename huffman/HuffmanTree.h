#pragma once
#include "misc.h"
#include "CodeTable.h"

class HuffmanTree
{
private:
	static const int maxTreeSize = 512; // is this good?
	int treeSize = 0;
	int alphabetSize = 0;
	int mTreeTable[maxTreeSize][2];
	void exploreNode(int nodeID, unsigned long long code,
		int length, CodeTable& codeTable);

public:
	HuffmanTree();
	~HuffmanTree();
	bool build(int *charStats, bool verbose = false);
	void writeCodes(CodeTable& codeTable);
	void print();
};

