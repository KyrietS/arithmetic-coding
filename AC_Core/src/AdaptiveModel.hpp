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
		: frequencies( numberOfSymbols ), 
		freqBegin( numberOfSymbols ),
		freqEnd( numberOfSymbols ),
		MAX_TOTAL_FREQUENCY(maxTotalFrequency)
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

		updateFrequencies();
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

		updateFrequencies();
	}

	size_t frequencyBegin(size_t symbol)
	{
		return freqBegin.at(symbol);
	}

	size_t frequencyEnd(size_t symbol)
	{
		return freqEnd.at(symbol);
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
	std::vector<size_t> freqBegin;
	std::vector<size_t> freqEnd;

	size_t totalFrequencyCounter;	// total number of symbols appearance.
	const size_t MAX_TOTAL_FREQUENCY;

	void updateFrequencies()
	{
		size_t prevFrequency = 0;
		for (size_t symbol = 0; symbol < frequencies.size(); symbol++)
		{
			freqBegin[symbol] = prevFrequency;
			prevFrequency += frequencies[symbol];
			freqEnd[symbol] = prevFrequency;		// end = begin + frequency
		}
	}
};
