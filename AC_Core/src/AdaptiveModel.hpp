//
// Copyright (c) 2020 Sebastian Fojcik
//

#pragma once

#include <stdexcept>
#include <vector>

class AdaptiveModel
{
public:
	AdaptiveModel(size_t numberOfSymbols, size_t maxTotalFrequency)
		: frequencies( numberOfSymbols ), MAX_TOTAL_FREQUENCY(maxTotalFrequency)
	{
		reset();
	}

	void reset()
	{
		for (int i = 0; i < frequencies.size(); i++)
			frequencies[i] = 1uL;	// initially every byte is marked as 'appeared once'

		totalFrequencyCounter = frequencies.size();

		if (totalFrequencyCounter >= MAX_TOTAL_FREQUENCY)
			throw std::invalid_argument("max frequency is too low to fit that number of symbols");
	}

	void update(size_t symbol)
	{
		if (totalFrequencyCounter >= MAX_TOTAL_FREQUENCY) {		// Check if frequency counter reaches max
			totalFrequencyCounter = 0;							// If so, halve all counts (keeping them
			for (auto& freq : frequencies) {					// positive).
				freq = freq / 2 != 0 ? freq / 2 : 1;			
				totalFrequencyCounter += freq;
			}
		}

		frequencies.at(symbol)++;
		totalFrequencyCounter++;
	}

	size_t frequencyBegin(size_t symbol)
	{
		if (symbol >= frequencies.size())
			throw std::out_of_range("Symbol doesn't belong to model");

		size_t result = 0;
		for (size_t i = 0; i < symbol; i++) 
			result += frequencies[i];

		return result;
	}

	size_t frequencyEnd(size_t symbol)
	{
		return frequencyBegin(symbol) + frequencies.at(symbol);
	}

	size_t totalFrequency()
	{
		return totalFrequencyCounter;
	}

	size_t size()
	{
		return frequencies.size();
	}

private:
	std::vector<size_t> frequencies;

	size_t totalFrequencyCounter;	// total number of symbols appearance.
	const size_t MAX_TOTAL_FREQUENCY;
};
