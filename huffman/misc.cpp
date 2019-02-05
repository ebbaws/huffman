#include <string>
#include <fstream>
#include <iostream>
#include "misc.h"
//using namespace std;

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

bool compare(int pivotVal, int compareVal, int pivotIdx, int compareIdx) {

	if (compareVal > pivotVal) {
		return true;
	}
	else if (compareVal == pivotVal) {
		return (compareIdx < pivotIdx);
	}
	else return false;

}

void doSort(int *values, int *indexes, int start, int end)
{
	if (start < end) {
		int pivotIdx = indexes[start];
		int pivotVal = values[pivotIdx];
		int pivotPos = start;
		int end2 = end;
		int compareIdx;
		int compareVal;
		for (int j = 0; j < (end - start); j++) {
			compareIdx = indexes[pivotPos + 1];
			compareVal = values[compareIdx];
			if (compare(pivotVal, compareVal, pivotIdx, compareIdx))
			{
				indexes[pivotPos] = compareIdx;
				pivotPos++;
			}
			else {
				indexes[pivotPos + 1] = indexes[end2];
				indexes[end2] = compareIdx;
				end2--;
			}
		}
		indexes[pivotPos] = pivotIdx;
		doSort(values, indexes, start, pivotPos - 1);
		doSort(values, indexes, pivotPos + 1, end);
		return;
	}
	else {
		return;
	}

}

void flip(int *myArray, int start, int end) {
	int idx = start;
	int swapIdx = end;
	while (idx < swapIdx) {
		int swapValue = myArray[swapIdx];
		myArray[swapIdx] = myArray[idx];
		myArray[idx] = swapValue;
		idx++;
		swapIdx--;
	}

}


// Puts indexes, sorted based on provided values, in array indicated by
// indexes argument (doesn't have to be initialized in advance,
// just provide a valid pointer)
// In case of equality between values, the original order of indexes
// will be preserved.
void mySort(int *values, int *indexes, int length, bool ascending) {
	// Initialize indexes
	for (int i = 0; i < length; i++) {
		indexes[i] = i;
	}
	
	doSort(values, indexes, 0, length - 1);

	if (ascending) {
		
		// Flip all indexes
		flip(indexes, 0, length - 1);

		// Flip the indexes within clusters of same value
		bool repeating = false;
		int repeatStart, repeatEnd;
		for (int i = 1; i < length; i++) {
			if (values[indexes[i]] == values[indexes[i - 1]]) {
				if (!repeating) {
					repeating = true;
					repeatStart = i - 1;
				}
				
				if (repeating&&(i == length - 1)) {
					repeatEnd = i;
					flip(indexes, repeatStart, repeatEnd);
				}
			}
			else {
				if (repeating) {
					repeating = false;
					repeatEnd = i - 1;
					flip(indexes, repeatStart, repeatEnd);
				}
			}

		}
		
	}

	//cout << "Sorted the indexes" << endl;
	//for (int i = 0; i < length; i++) {
	//	cout << indexes[i] << "\t" << values[indexes[i]] << endl;

	//}

}

void testSort() {
	int indexes[10];
	int values[10] = { 0, 4, 3, 0, 3, 0, 3, 0, 3, 4 };
	mySort(values, indexes, 10, true);

}

int intFromChar(char myChar) {
	if (myChar < 48 || myChar > 57) {
		return -1;
	} else {
		return static_cast<int>(myChar - 48);
	}
}

int stringToPositiveInt(std::string & myString)
{
	int result = 0;

	int stringSize = static_cast<int>(myString.size());

	for (int i = 0; i < stringSize; i++) {
		int digit = intFromChar(myString.at(i));
		if (digit < 0) {
			return -1;
		}
		else {
			result = result * 10 + digit;
		}
	}

	return result;
}

bool compareFiles(std::string & fileName1, std::string & fileName2) {
	using std::cout;
	using std::endl;
	using std::ios;
	
	cout << "Comparing files... ";
	std::ifstream file1(fileName1, ios::in | ios::binary | ios::ate);
	std::ifstream file2(fileName2, ios::in | ios::binary | ios::ate);

	if (file1.is_open() && file2.is_open()) {

		file1.seekg(0, ios::beg);
		file2.seekg(0, ios::beg);

		char byte1, byte2;

		bool success = false;

		while (true) {
			file1.get(byte1);
			file2.get(byte2);

			if (byte1 != byte2) {
				cout << "Found inconsistency, current position: " <<
					file1.tellg() << endl;
				break;
			}

			
			if (file1.eof()) {
				if (file2.eof()) {
					success = true;
				}
				else {
					cout << fileName1 << " ended before " << fileName2 << "!" << endl;
				}
				break;
			}

			if (file2.eof()) {
				cout << fileName2 << " ended before " << fileName1 << "!" << endl;
				break;
			}
		}
		
		file1.close();
		file2.close();

		if (success) {
			cout << "Reached end of both files with no inconsistencies" << endl;
		}
		
		return success;

	}
	else {
		cout << "At least one of the files is unreadable" << endl;
		return false;
	}


}
