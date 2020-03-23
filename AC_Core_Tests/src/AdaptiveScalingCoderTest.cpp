#include <catch2/catch.hpp>
#include "AdaptiveScalingCoder.hpp"
#include <fstream>
#include <filesystem>
#include <algorithm>

#pragma warning( disable : 6237 6319 )

namespace fs = std::filesystem;

bool files_equal(std::string& path1, std::string& path2)
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

SCENARIO("Encoding test") {
	AdaptiveScalingCoder coder;
	std::string encode_path_in = "_encode_in.test.tmp";
	std::string encode_path_out = "_encode_out.test.tmp";
	std::string decode_path_in = "_decode_in.test.tmp";
	std::string decode_path_out = "_decode_out.test.tmp";
	fs::remove(encode_path_in);
	fs::remove(encode_path_out);
	fs::remove(decode_path_in);
	fs::remove(decode_path_out);

	GIVEN("file to encode and filepath to save result") {
		std::ofstream file(encode_path_in);
		file << "abc";
		file.close();

		WHEN("input file is encoded") {
			coder.encode(encode_path_in, encode_path_out);

			THEN("output file is created") {
				CHECK(fs::exists(encode_path_out));
			}
		}
	}
	GIVEN("file with content") {
		std::ofstream file(encode_path_in);
		file << "abc";
		file.close();

		WHEN("input file is encoded") {
			coder.encode(encode_path_in, encode_path_out);
			REQUIRE(fs::exists(encode_path_out));

			THEN("output file can be decoded") {
				coder.decode(encode_path_out, decode_path_out);

				CHECK(files_equal(encode_path_in, decode_path_out) == true);
			}
		}
	}

	fs::remove(encode_path_in);
	fs::remove(encode_path_out);
	fs::remove(decode_path_in);
	fs::remove(decode_path_out);
}
