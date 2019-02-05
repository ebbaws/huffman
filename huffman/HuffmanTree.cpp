#include "HuffmanTree.h"
#include <iostream>
#include "misc.h"
//using namespace std;

void HuffmanTree::exploreNode(int nodeID, int length, CodeTable & codeTable)
{
	int leftID = mTreeTable[nodeID][0];
	int rightID = mTreeTable[nodeID][1];
	
	if ((leftID == -1) && (rightID == -1)) {
		// Node is leaf; write code to table
		//codeTable.setCode(nodeID, code, length);
		codeTable.setLength(nodeID, length);
	}
	else {
		// Explore children
		exploreNode(leftID, length + 1, codeTable);
		exploreNode(rightID, length + 1, codeTable);
	}

}

HuffmanTree::HuffmanTree()
{
}

HuffmanTree::~HuffmanTree()
{
}

// Build the Huffman tree. The charStats arg should point to an 
// array of the # of occurrences for each character.
bool HuffmanTree::build(int *charStats, bool verbose) {
	
	alphabetSize = maxAlphabetSize;
	int nodeWeights[maxTreeSize];
	int indexes[maxAlphabetSize];
	mySort(charStats, indexes, maxAlphabetSize);

	using std::cout;
	using std::endl;

	if (verbose) {
		cout << "Sorted char statistics: " << endl;
		for (int i = 0; i < maxAlphabetSize; i++) {
			cout << i << "\t" << charStats[indexes[i]] << "\t" <<
				indexes[i] << endl;
		}
	}

	// Determine number of non-zero-occurrence characters
	// (only these characters will be included in tree)
	while (charStats[indexes[alphabetSize - 1]] == 0)
	{
		// could the following be replaced with indexes[--alphSize] = -1?
		indexes[alphabetSize - 1] = -1;
		alphabetSize--;
	}

	if (verbose) {
		cout << "Actual alphabet size: " << alphabetSize << endl;
	}

	// Initialize the Huffman tree with leaf nodes representing
	// the symbols (zero-occurrence symbols will be included here,
	// but will not be linked in the tree)
	treeSize = maxAlphabetSize;
	for (int j = 0; j < treeSize; j++)
	{
		mTreeTable[j][0] = -1;
		mTreeTable[j][1] = -1;
		nodeWeights[j] = charStats[j];
	}

	int end = alphabetSize - 1;
	
	// Build the Huffman tree
	while (end > 0) 
	{
		if (treeSize < maxTreeSize)
		{
			// Create new node
			mTreeTable[treeSize][0] = indexes[end - 1];
			mTreeTable[treeSize][1] = indexes[end];
			nodeWeights[treeSize] = nodeWeights[indexes[end - 1]] +
				nodeWeights[indexes[end]];

			// Replace the two last nodes in indexes with the new one
			indexes[end] = -1;
			indexes[end - 1] = treeSize;
			end--;

			// Step through indexes vector and move the new node to the left
			// until the nodes are sorted by weight again
			int currentIndex = end - 1;
			while ((currentIndex >= 0) &&
				(nodeWeights[treeSize] > nodeWeights[indexes[currentIndex]]))
			{
				indexes[currentIndex + 1] = indexes[currentIndex];
				indexes[currentIndex] = treeSize;
				currentIndex--;
			}

			treeSize++;

		}
		else {
			cout << "Max tree size exceeded (unexpected error)!" << endl;
			return false;
		}
	}
	cout << "Tree successfully built" << endl;
	return true;
}

void HuffmanTree::writeCodeLengths(CodeTable &codeTable)
{
	exploreNode(treeSize - 1, 0, codeTable);
}

void HuffmanTree::print()
{
	std::cout << "Huffman tree table: " << std::endl;
	for (int i = 0; i < treeSize; i++) {
		std::cout << i << ":\t" << mTreeTable[i][0] << "\t" <<
			mTreeTable[i][1] << std::endl;
	}
}
