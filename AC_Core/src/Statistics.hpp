#pragma once

#include <vector>

#include "BitUtils/BitStat.hpp"

class Statistics
{
public:
	Statistics(std::vector<BitStat> stats)
		: m_stats(stats) {}

	double entropy()
	{
		unsigned long long n = 0uLL;	// total number of bits written
		for (size_t i = 0; i < m_stats.size(); i++)
			n += m_stats[i].readCounter;

		double h = 0;
		for (size_t i = 0; i < m_stats.size(); i++)
		{
			double p = (double)m_stats[i].readCounter / n;
			if (p > 0)
				h += m_stats[i].readCounter * -log2(p);
		}
		return h / n;
	}

	double averageCodingLength()
	{
		unsigned long long inputBits = 0;
		unsigned long long outputBits = 0;

		for (size_t i = 0; i < m_stats.size(); i++)
		{
			inputBits += m_stats[i].readCounter;
			outputBits += m_stats[i].writeCounter;
		}

		return 8 * ((double)outputBits / inputBits);	// bits per symbol (byte)
	}

	double compressionRatio()
	{
		unsigned long long inputBits =  0;
		unsigned long long outputBits = 0;

		for (size_t i = 0; i < m_stats.size(); i++)
		{
			inputBits += m_stats[i].readCounter;
			outputBits += m_stats[i].writeCounter;
		}

		return (1.0 - (double)outputBits / inputBits);
	}

private:
	std::vector<BitStat> m_stats;
};