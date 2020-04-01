#pragma once

// Container for bits statistics.
struct BitStat
{
	unsigned long long readCounter = 0;		// Number of input bits.
	unsigned long long writeCounter = 0;	// Number of output bits.
};
