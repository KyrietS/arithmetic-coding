#include <catch2/catch.hpp>
#include "BitUtils/BitWriter.hpp"
#include "BitUtils/BitReader.hpp"

#include <filesystem>
#pragma warning( disable : 6237 6319 )

namespace fs = std::filesystem;

std::string path = ".file.test.tmp";

// Tests for interoperability between BitWriter and BitReader.
//
// If program uses both BitWriter to save bits and BitReader
// to read them from a file then it doesn't matter how bits 
// are stored in a file. The only thing that matters is that 
// they (bits) have to be read in the same order they were 
// stored (written).

SCENARIO("Bits order is kept")
{
	fs::remove(path);
	BitWriter writer(path);

	WHEN("BitWriter writes a sequence of bits")
	{
		writer.write(0);
		writer.write(1);
		writer.write(1);
		writer.write(0);
		writer.write(1);

		writer.flush(); // File contains one byte (padded with zeroes).
						// For example: 0110'1000
						// Actualy it's not important how this byte is stored.

		THEN("BitReader reads the sequence in the same order it was written")
		{
			BitReader reader(path);
			CHECK(reader.read() == false); // 0
			CHECK(reader.read() == true);  // 1
			CHECK(reader.read() == true);  // 1
			CHECK(reader.read() == false); // 0
			CHECK(reader.read() == true);  // 1
			// padding can also be read
			CHECK(reader.eof() == false);
			CHECK(reader.read() == false);
			CHECK(reader.read() == false);
			CHECK(reader.read() == false);
			CHECK(reader.eof() == true);
		}
	}
}