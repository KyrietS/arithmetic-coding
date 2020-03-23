#include <catch2/catch.hpp>
#include <string>
#include <fstream>
#include <filesystem>
#include "BitUtils/BitReader.hpp"

#pragma warning( disable : 6237 6319 )

namespace fs = std::filesystem;

SCENARIO("BitReader opens a file", "[BitReader]") {
	GIVEN("A file name to existing file") {
		std::string filename = "_file_1.test.tmp";
		std::ofstream file(filename);
		file.close();

		WHEN("BitReader object is created") {
			BitReader reader(filename);

			THEN("file is open") {
				SUCCEED();
			}
		}

		fs::remove(filename);
	}
	GIVEN("A file name to non-existent file") {
		std::string nonexistentFilename = "_file_1.test.tmp";
		fs::remove(nonexistentFilename);

		WHEN("BitReader object is created") {
			THEN("An error is thrown") {
				REQUIRE_THROWS(BitReader(nonexistentFilename));
			}
		}
	}
}

SCENARIO("BitReader reads data from a file") {
	GIVEN("A file with data in it") {
		std::string filename = "_file_1.test.tmp";
		std::ofstream file(filename, std::ios_base::binary);
		file.put(0b0000'1101);
		file.close();

		AND_GIVEN("BitReader instance")
		{
			BitReader reader(filename);

			WHEN("data is read") {
				bool bit0 = reader.read();
				bool bit1 = reader.read();
				bool bit2 = reader.read();
				bool bit3 = reader.read();
				bool bit4 = reader.read();

				THEN("it's the same as in the file") {
					CHECK(bit0 == true);  // 1
					CHECK(bit1 == false); // 0
					CHECK(bit2 == true);  // 1
					CHECK(bit3 == true);  // 1
					CHECK(bit4 == false); // 0
				}
			}
			WHEN("more data than a file contains is read") {
				REQUIRE(reader.eof() == false);
				for (int i = 0; i < 100; i++) {
					reader.read();
				}

				THEN("reader returns 0 bit as a result") {
					CHECK(reader.read() == false);
				}
				THEN("EOF flag is set") {
					CHECK(reader.eof() == true);
				}
			}
		}
		fs::remove(filename);
	}
	GIVEN("An empty file") {
		std::string filename = "_file_1.test.tmp";
		std::ofstream emptyFile(filename);
		emptyFile.close();

		WHEN("BitReader object is created") {
			BitReader reader(filename);

			THEN("EOF flag is set") {
				CHECK(reader.eof() == true);
			}
		}
	}
}