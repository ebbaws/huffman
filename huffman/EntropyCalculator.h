#ifndef ENTROPYCALCULATOR_H
#define ENTROPYCALCULATOR_H
#include <string>

class EntropyCalculator
{
public:
	EntropyCalculator();
	~EntropyCalculator();
	double calcMemorylessEntropy(std::string filePath);
	double calcHigherOrderEntropy(std::string filePath, int order);

};

#endif