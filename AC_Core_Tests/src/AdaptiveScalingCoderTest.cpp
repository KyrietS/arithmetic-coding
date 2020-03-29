//
// Copyright (c) 2020 Sebastian Fojcik
//

#include <catch2/catch.hpp>
#include "AdaptiveScalingCoder.hpp"
#include <fstream>
#include <filesystem>
#include <algorithm>

#pragma warning( disable : 6237 6319 )

namespace fs = std::filesystem;

bool files_equal(const std::string& path1, const std::string& path2); // checks if files' contents are equal

// --- temp files paths for use in tests ---
const std::string encode_path_in = "_encode_in.test.tmp";
const std::string encode_path_out = "_encode_out.test.tmp";
const std::string decode_path_in = "_decode_in.test.tmp";
const std::string decode_path_out = "_decode_out.test.tmp";

void removeAllTempFiles()
{
	fs::remove(encode_path_in);
	fs::remove(encode_path_out);
	fs::remove(decode_path_in);
	fs::remove(decode_path_out);
}

SCENARIO("Encoding creates new file", "[Coder]") 
{
	removeAllTempFiles();
	AdaptiveScalingCoder coder;

	GIVEN("a file to encode") {
		std::ofstream(encode_path_in) << "abc";

		WHEN("input file is encoded") {
			coder.encode(encode_path_in, encode_path_out);

			THEN("output file is created") {
				CHECK(fs::exists(encode_path_out));
			}
		}
	}
}

SCENARIO("Decoding creates new file", "[Coder]")
{
	removeAllTempFiles();
	AdaptiveScalingCoder coder;

	GIVEN("an encoded file to decode") {
		std::ofstream(encode_path_in) << "some data";
		coder.encode(encode_path_in, decode_path_in);

		WHEN("file is decoded") {
			coder.decode(decode_path_in, decode_path_out);

			THEN("new file is created") {
				CHECK(fs::exists(decode_path_out));
			}
		}
	}
	removeAllTempFiles();
}

SCENARIO("Processing text files", "[Coder]")
{
	removeAllTempFiles();
	AdaptiveScalingCoder coder;

	GIVEN("a text file with short content") {
		std::ofstream(encode_path_in) << "abcdefghijkl0123456789ABCDE";

		WHEN("input file is encoded") {
			coder.encode(encode_path_in, encode_path_out);
			REQUIRE(fs::exists(encode_path_out));

			THEN("output file can be decoded") {
				coder.decode(encode_path_out, decode_path_out);

				CHECK(files_equal(encode_path_in, decode_path_out));
			}
		}
	}
	GIVEN("a text file with long content")
	{
		std::ofstream file(encode_path_in);
		for (int i = 1; i <= 50; i++) {
			file << std::string(i, 'a') << std::string(i, 'b') << std::string(i, 'c') << '\n';
		}
		file.close();

		WHEN("input file is encoded") {
			coder.encode(encode_path_in, encode_path_out);
			REQUIRE(fs::exists(encode_path_out));

			THEN("output file can be decoded") {
				coder.decode(encode_path_out, decode_path_out);

				CHECK(files_equal(encode_path_in, decode_path_out));
			}
		}
	}
	removeAllTempFiles();
}

SCENARIO("Processing binary files", "[Coder]")
{
	removeAllTempFiles();
	AdaptiveScalingCoder coder;

	GIVEN("a small binary file")
	{
		std::ofstream(encode_path_in, std::ofstream::binary)
			.put(0)
			.put((char)255);

		WHEN("input file is encoded") {
			coder.encode(encode_path_in, encode_path_out);
			REQUIRE(fs::exists(encode_path_out));

			THEN("output file can be decoded") {
				coder.decode(encode_path_out, decode_path_out);

				CHECK(files_equal(encode_path_in, decode_path_out));
			}
		}
	}
	GIVEN("a big binary file")
	{
		std::ofstream file(encode_path_in, std::ofstream::binary);
		for (int chunk = 0; chunk < 10; chunk++) {
			for (int byte = 0; byte < 256; byte++) {
				file.put(byte);
			}
		}
		file.close();
		
		WHEN("input file is encoded") {
			coder.encode(encode_path_in, encode_path_out);
			REQUIRE(fs::exists(encode_path_out));

			THEN("output file can be decoded") {
				coder.decode(encode_path_out, decode_path_out);

				CHECK(files_equal(encode_path_in, decode_path_out));
			}
		}
	}
	removeAllTempFiles();
}


bool files_equal(const std::string& path1, const std::string& path2)
{
	if (!fs::exists(path1) || !fs::exists(path2))
		return false;	// one of the files doesn't exists

	if (fs::file_size(path1) != fs::file_size(path2))
		return false;	// files have different size

	std::ifstream file1(path1, std::ifstream::binary);
	std::ifstream file2(path1, std::ifstream::binary);
	std::istreambuf_iterator<char> begin1(file1);
	std::istreambuf_iterator<char> begin2(file2);

	return std::equal(begin1, std::istreambuf_iterator<char>(), begin2); //Second argument is end-of-range iterator
}