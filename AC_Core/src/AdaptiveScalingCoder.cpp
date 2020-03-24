//
// Copyright (c) 2020 Sebastian Fojcik
//

#include "AdaptiveScalingCoder.hpp"
#include "BitUtils/BitWriter.hpp"
#include "BitUtils/BitReader.hpp"

#include <cstdint>
#include <fstream>

constexpr uint64_t powerOf(uint64_t a, uint64_t n)
{
	return n == 0 ? 1 : a * powerOf(a, n - 1);
}

/* Parameters for encoding algorithm */
static constexpr uint64_t PRECISION = 32;
static constexpr uint64_t WHOLE = powerOf(2, PRECISION);
static constexpr uint64_t HALF = WHOLE / 2;
static constexpr uint64_t QUARTER = WHOLE / 4;

using byte_t = uint8_t;

void AdaptiveScalingCoder::encode(std::string path_in, std::string path_out)
{
	std::ifstream in(path_in, std::ifstream::binary);
	BitWriter out(path_out);
	uint64_t a = 0L;
	uint64_t b = WHOLE;
	int s = 0L;

	bool endOfInput = false;
	while (!endOfInput)
	{
		int byte = in.get();
		if (byte == EOF) {
			endOfInput = true;
			byte = TOTAL_NUMBER_OF_SYMBOLS - 1; // last symbol is EOF symbol
		}

		size_t symbol = byte;
		uint64_t w = b - a;
		b = a + llround(w * ((double)model.frequencyEnd(symbol) / model.frequencyTotal()));
		a = a + llround(w * ((double)model.frequencyBegin(symbol) / model.frequencyTotal()));

		// Scaling left or right
		while (b < HALF || a > HALF)
		{
			if (b < HALF) {
				out.write(0);
				out.writeN(1, s);
				s = 0;
				a *= 2;
				b *= 2;
			}
			else if (a > HALF) {
				out.write(1);
				out.writeN(0, s);
				s = 0;
				a = 2 * (a - HALF);
				b = 2 * (b - HALF);
			}
		}

		// Scaling "blow up"
		while (a > QUARTER && b < 3 * QUARTER) 
		{
			s += 1;
			a = 2 * (a - QUARTER);
			b = 2 * (b - QUARTER);
		}
	}

	s += 1;
	if (a <= QUARTER) {
		out.write(0);
		out.writeN(1, s);
	}
	else {
		out.write(1);
		out.writeN(0, s);
	}
}

void AdaptiveScalingCoder::decode(std::string path_in, std::string path_out)
{
	BitReader in(path_in);
	std::ofstream out(path_out, std::ofstream::binary);

	uint64_t a = 0L;
	uint64_t b = WHOLE;
	uint64_t z = 0L;

	size_t i = 1;
	while (i <= PRECISION && !in.eof()) {
		if (in.read())	// bit '1' read
			z += powerOf(2, PRECISION - i);
		i += 1;
	}

	while (true)
	{
		// try to decode a symbol
		for (int symbol = 0; symbol < TOTAL_NUMBER_OF_SYMBOLS; symbol++)
		{
			uint64_t w = b - a;
			uint64_t b0 = a + llround(w * ((double)model.frequencyEnd(symbol) / model.frequencyTotal()));
			uint64_t a0 = a + llround(w * ((double)model.frequencyBegin(symbol) / model.frequencyTotal()));

			// Symbol can be decoded
			if (a0 <= z && z < b0) {
				if (symbol == TOTAL_NUMBER_OF_SYMBOLS - 1) { // End Of File symbol
					return;
				}

				byte_t decoded = (byte_t)symbol;
				out.put(decoded);

				a = a0;
				b = b0;

				break;
			}
		}

		// Scaling left or right
		while (b < HALF || a > HALF) 
		{
			if (b < HALF) {
				a *= 2;
				b *= 2;
				z *= 2;
			}
			else if (a > HALF) {
				a = 2 * (a - HALF);
				b = 2 * (b - HALF);
				z = 2 * (z - HALF);
			}

			// Update z approximation
			if (!in.eof() && in.read())
				z += 1;
			i += 1;
		}

		// Scaling "blow up"
		while (a > QUARTER && b < 3 * QUARTER)
		{
			a = 2 * (a - QUARTER);
			b = 2 * (b - QUARTER);
			z = 2 * (z - QUARTER);

			// Update z approximation
			if (!in.eof() && in.read())
				z += 1;
			i += 1;
		}
	}
}
