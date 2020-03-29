//
// Copyright (c) 2020 Sebastian Fojcik
//

#include "AdaptiveScalingCoder.hpp"
#include "AdaptiveModel.hpp"
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

/* Parameters for data model */
static constexpr int MODEL_SIZE = 256 + 1;					// 256 + 1 for EOF symbol
static constexpr int MODEL_EOF_SYMBOL = 256;				// last symbol in model is EOF symbol
static constexpr int MODEL_MAX_FREQUENCY = QUARTER - 1;		// max total frequency for model

using byte_t = uint8_t;

void AdaptiveScalingCoder::encode(std::string path_in, std::string path_out)
{
	std::ifstream in(path_in, std::ifstream::binary);
	BitWriter out(path_out);
	AdaptiveModel model(MODEL_SIZE, MODEL_MAX_FREQUENCY);

	uint64_t a = 0L;
	uint64_t b = WHOLE;
	int s = 0L;

	bool endOfInput = false;
	while (!endOfInput)
	{
		int byte = in.get();
		if (byte == EOF) {
			endOfInput = true;
			byte = MODEL_EOF_SYMBOL; // get representation of EOF symbol
		}

		size_t symbol = byte;
		uint64_t w = b - a;
		b = a + llround(w * ((double)model.frequencyEnd(symbol) / model.totalFrequency()));
		a = a + llround(w * ((double)model.frequencyBegin(symbol) / model.totalFrequency()));

		// Scaling
		while (true)
		{
			if (b < HALF) {			// Expand left. [a = 2a, b = 2b]
				out.write(0);
				out.writeN(1, s);
				s = 0;
			}
			else if (a > HALF) {	// Expand right. [a = 2(a-half), b = 2(b-HALF)]
				out.write(1);
				out.writeN(0, s);
				s = 0;
				a -= HALF;
				b -= HALF;
			}
			else if (a > QUARTER && b < 3 * QUARTER) {	// Expand middle (blow up). [a = 2(a-quarter), b = 2(b-quarter)]
				s += 1;
				a -= QUARTER;
				b -= QUARTER;
			}
			else {		// No more scaling.
				break;	// At this point [a,b] range is at least HALF in length.
			}
			a *= 2;
			b *= 2;
		}
		
		model.update(symbol);
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
	AdaptiveModel model(MODEL_SIZE, MODEL_MAX_FREQUENCY);

	uint64_t a = 0L;
	uint64_t b = WHOLE;
	uint64_t z = 0L;

	size_t i = 1;
	while (i <= PRECISION && !in.eof()) {		// Initialize 'z' with as many bits as you can
		if (in.read())	// bit '1' read
			z += powerOf(2, PRECISION - i);
		i += 1;
	}

	while (true)
	{
		// try to decode a symbol
		for (int symbol = 0; symbol < model.size(); symbol++)
		{
			uint64_t w = b - a;
			uint64_t b0 = a + llround(w * ((double)model.frequencyEnd(symbol) / model.totalFrequency()));
			uint64_t a0 = a + llround(w * ((double)model.frequencyBegin(symbol) / model.totalFrequency()));

			// Symbol can be decoded
			if (a0 <= z && z < b0) {
				if (symbol == MODEL_EOF_SYMBOL) { // End Of File symbol
					return;
				}

				byte_t decoded = (byte_t)symbol;
				out.put(decoded);

				a = a0;
				b = b0;

				model.update(decoded);
				break;
			}
		}

		// Scaling
		while (true) 
		{
			if (b < HALF) {				// Expand left
				/* nothing */
			}
			else if (a > HALF) {		// Expand right
				a -= HALF;
				b -= HALF;
				z -= HALF;
			}
			else if (a > QUARTER && b < 3 * QUARTER) {	// Expand middle (blow up)
				a -= QUARTER;
				b -= QUARTER;
				z -= QUARTER;
			}
			else {		// No more scaling.
				break;	// At this point [a,b] range is at least HALF in length.
			}
			a *= 2;
			b *= 2;
			z *= 2;

			// Update z approximation
			if (!in.eof() && in.read())
				z += 1;
			i += 1;
		}
	}
}
