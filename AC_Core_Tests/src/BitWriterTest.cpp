#include <catch2/catch.hpp>
#include <string>
#include <filesystem>
#include <fstream>
#include "BitUtils/BitWriter.hpp"

#pragma warning( disable : 6237 6319 )

namespace fs = std::filesystem;

SCENARIO("BitWriter creates and opens file", "[BitWriter]") {
	GIVEN("A file name") {
		std::string filename = "_file_1.test.tmp";
		fs::remove(filename);

		WHEN("File doesn't exist") {
			REQUIRE_FALSE(fs::exists(filename));

			THEN("BitWriter creates new file with given name") {
				BitWriter bw(filename);

				CHECK(fs::exists(filename));
			}
		}
		WHEN("File does exists") {
			std::ofstream someFile(filename);
			someFile << "Some data";
			someFile.close();

			REQUIRE(fs::exists(filename));

			THEN("BitWriter won't overwrite it and throw an error") {
				REQUIRE_THROWS_WITH(BitWriter(filename), Catch::Contains("name already exists"));
			}
		}
		fs::remove(filename);
	}
	GIVEN("An incorrect file name") {
		std::string incorrectFilename = GENERATE("", "@#$%^&*?", "\0a\0");

		WHEN("BitWriter tries to create it") {
			THEN("an error is thrown") {
				REQUIRE_THROWS(BitWriter(incorrectFilename));
				REQUIRE_FALSE(fs::exists(incorrectFilename));
			}
		}
	}
}

SCENARIO("BitWriter writes data to a file", "[BitWriter]") {
	std::string filename = "_file_1.test.tmp";
	fs::remove(filename);

	GIVEN("A BitWriter object") {
		BitWriter writer(filename);

		WHEN("one byte 'a' is written") {
			// 'a' (0x61) <-- bits are read from right to left
			writer << 1 << 0 << 0 << 0;		// 0001
			writer << 0 << 1 << 1 << 0;     // 0110
			writer.flush();

			THEN("file contains that byte 'a'") {
				std::ifstream file(filename, std::ios_base::binary);
				char letterA;
				file.read(&letterA, 1);

				CHECK(letterA == 'a');
			}
		}
		WHEN("less than a byte is written") {
			writer << 1 << 0 << 1 << 1;
			writer.flush();

			THEN("file contains one byte padded with '0' bits") {
				std::ifstream file(filename, std::ios_base::binary);
				char byte;
				file.read(&byte, 1);

				CHECK(byte == 0b0000'1101);
			}
		}
		WHEN("more than a byte is written") {
			writer << 1 << 1 << 0 << 0;
			writer << 0 << 1 << 1 << 0;
			writer << 1 << 1 << 1;
			writer.flush();

			THEN("file containes two bytes padded with '0' bits") {
				std::ifstream file(filename, std::ifstream::binary);
				char firstByte = file.get();
				char secondByte = file.get();

				CHECK(firstByte == 0b0110'0011);
				CHECK(secondByte == 0b0000'0111);
			}
		}
		WHEN("six same bits are written") {
			writer.writeN(1, 6);
			writer.flush();

			THEN("file contains six '1' bits") {
				std::ifstream file(filename, std::ios_base::binary);
				char byte = file.get();

				CHECK(byte == 0b0011'1111);
			}
		}
	}
	fs::remove(filename);
}

SCENARIO("BitWriter collects stats about bits being written", "[BitWriter]")
{
	std::string filename = "_file_1.test.tmp";
	fs::remove(filename);

	GIVEN("A BitWriter object") {
		BitWriter writer(filename);

		WHEN("Compressed byte 0xFF is written")
		{
			writer.beginByte(0xFF);
			writer << 1 << 1 << 1 << 1;		// Byte written as 0x0F
			writer.flush();
			std::vector<BitStat> stats = writer.getStats();

			THEN("writer collected stats about this byte") {
				CHECK(stats.at(0xFF).readCounter == 8);
				CHECK(stats.at(0xFF).writeCounter == 4);
			}

			THEN("stats about other bytes are not updated") {
				CHECK(stats.at(0x00).readCounter == 0);
				CHECK(stats.at(0x00).writeCounter == 0);
				CHECK(stats.at(0xAB).readCounter == 0);
				CHECK(stats.at(0xAB).writeCounter == 0);
			}
		}
		WHEN("nonexistent byte is begun")
		{
			writer.beginByte(0xFFF);
			writer << 1 << 1;
			writer.beginByte(-1);
			writer << 0 << 0;
			writer.flush();

			THEN("no stats are updated") {
				std::vector<BitStat> emptyStats = writer.getStats();
				unsigned long long sum = 0;
				for (auto s : emptyStats)
					sum += s.readCounter + s.writeCounter;
				CHECK(sum == 0);
			}
		}
	}

	fs::remove(filename);
}
