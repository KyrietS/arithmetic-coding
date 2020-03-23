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

		WHEN("File doesn't exists") {
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
	GIVEN("A BitWriter object") {
		std::string filename = "_file_1.test.tmp";
		fs::remove(filename);
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

			THEN("file contains one byte padded with 0 bits") {
				std::ifstream file(filename, std::ios_base::binary);
				char byte;
				file.read(&byte, 1);

				CHECK(byte == 0b0000'1101);
			}
		}
	}
}
