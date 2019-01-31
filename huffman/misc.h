#ifndef MISC_H
#define MISC_H

#include <string>

// Apparently this will work for defining project-wide constants
// (as long as you include this header everywhere you need them)
// Let's run with it for now.
const int maxAlphabetSize = 256;

int char2index(char c);
char index2char(int index);
void mySort(int *values, int *indexes, int length, bool ascending = false);
bool compareFiles(std::string & fileName1, std::string & fileName2);
void testSort();
int stringToPositiveInt(std::string & myString);

#endif