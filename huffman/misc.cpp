#include <string>
#include <fstream>
#include <iostream>
using namespace std;

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

void doSort(int *values, int *indexes, int start, int end, bool init) {
	if (init) {
		// Initialize indexes
		for (int i = 0; i <= end; i++) {
			*(indexes + i) = i;
		}
	}

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
			if (compareVal > pivotVal) {
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
		doSort(values, indexes, start, pivotPos - 1, false);
		doSort(values, indexes, pivotPos + 1, end, false);
		return;
	}
	else {
		return;
	}

}

// Puts indexes, sorted based on provided values, in array indicated by
// indexes argument (doesn't have to be initialized in advance,
// just provide a valid pointer)
void mySort(int *values, int *indexes, int length, bool ascending = false) {
	doSort(values, indexes, 0, length - 1, true);
	if (ascending) {
		// Flip the indexes
		int idx = 0;
		int swapIdx = length-1;
		while (idx < swapIdx) {
			int swapValue = indexes[swapIdx];
			indexes[swapIdx] = indexes[idx];
			indexes[idx] = swapValue;
			idx++;
			swapIdx--;
		}
	}
}

bool compareFiles(string & fileName1, string & fileName2) {
	ifstream file1(fileName1, ios::in | ios::binary | ios::ate);
	ifstream file2(fileName2, ios::in | ios::binary | ios::ate);

	if (file1.is_open() && file2.is_open()) {

		file1.seekg(0, ios::beg);
		file2.seekg(0, ios::beg);

		char byte1, byte2;

		bool success = false;

		while (true) {
			file1.get(byte1);
			file2.get(byte2);

			if (byte1 != byte2) {
				cout << "Found inconsistency" << endl;
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
				if (file2.eof()) {
					success = true;
				}
				else {
					cout << fileName2 << " ended before " << fileName1 << "!" << endl;
				}

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
