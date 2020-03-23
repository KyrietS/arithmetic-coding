#include <catch2/catch.hpp>
#include <string>
#include <filesystem>
#include <fstream>
#include "BitUtils/BitWriter.hpp"

#pragma warning( disable : 6237 6319 )


namespace fs = std::filesystem;

TEST_CASE("Create BitWriter and open new file", "[BitWriter]")
{
	SECTION("Create and open a new file")
	{
		std::string filename = "_file_1.test.tmp";
		fs::remove(filename);

		REQUIRE_NOTHROW(BitWriter(filename));
		REQUIRE(fs::exists(filename));

		fs::remove(filename);
	}
	SECTION("File already exists")
	{
		std::string filename = "_file_1.test.tmp";
		{
			fs::remove(filename);
			std::ofstream someFile(filename);
			someFile << "Some data";
		}

		REQUIRE_THROWS_WITH(BitWriter(filename), Catch::Contains("name already exists"));

		fs::remove(filename);
	}
	SECTION("File name is invalid")
	{
		std::string invalidFilename = GENERATE("", "@#$%^&*?", "\0a\0");
		REQUIRE_THROWS(BitWriter(invalidFilename));
	}
}

TEST_CASE("Write bits to a file", "[BitWriter]")
{
	SECTION("Write single byte (8 bits)")
	{
		std::string filename = "_file_1.test.tmp";
		fs::remove(filename);
		REQUIRE_FALSE(fs::exists(filename));

		{
			BitWriter writer(filename);
			// 'a' (0x61) <-- bits are read from right to left
			writer << 1 << 0 << 0 << 0;		// 0001
			writer << 0 << 1 << 1 << 0;     // 0110
			writer.flush();
		}

		REQUIRE(fs::exists(filename));
		std::ifstream file(filename);
		std::string letterA;
		file >> letterA;
		CHECK(letterA == "a");
	}
	SECTION("Write bits with padding")
	{
		std::string filename = "_file_1.test.tmp";
		fs::remove(filename);
		REQUIRE_FALSE(fs::exists(filename));

		{
			BitWriter writer(filename);
			writer << 1 << 0 << 1;
			writer.flush();
		}

		REQUIRE(fs::exists(filename));
		std::ifstream file(filename);
		byte_t byte;
		file >> byte;
		CHECK(byte == '\x05');
	}
}