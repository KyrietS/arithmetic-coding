#pragma once

#include <stdexcept>

constexpr unsigned int TOTAL_NUMBER_OF_SYMBOLS = 255 + 1;

class AlphabetModel
{
public:
	AlphabetModel()
		: totalFrequencyCounter(TOTAL_NUMBER_OF_SYMBOLS), frequencies{ 0 }
	{
		for (int i = 0; i < TOTAL_NUMBER_OF_SYMBOLS; i++)
			frequencies[i] = 1uL;	// every byte is marked as 'appeared once'
	}

	void update(size_t symbol)
	{
		frequencies[symbol]++;
		totalFrequencyCounter++;
	}

	unsigned long frequencyBegin(size_t symbol)
	{
		unsigned long result = 0;
		for (int i = 0; i < symbol; i++) 
			result += frequencies[i];

		return result;
	}

	unsigned long frequencyEnd(size_t symbol)
	{
		return frequencyBegin(symbol) + frequencies[symbol];
	}

	unsigned long frequencyTotal()
	{
		return totalFrequencyCounter;
	}

private:
	/* Frequency of a particular symbol. How many times it appeared. 
	   +1 for EOF symbol which doesn't have byte representation. */
	unsigned long frequencies[TOTAL_NUMBER_OF_SYMBOLS];

	unsigned long totalFrequencyCounter;	// total number of symbol apperance.
};
