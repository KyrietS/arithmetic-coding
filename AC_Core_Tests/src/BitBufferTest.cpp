#include <catch2/catch.hpp>
#include <stdexcept>
#include <cstdlib> // rand
#include <ctime> // time
#include "BitUtils/BitBuffer.hpp"

#pragma warning( disable : 6237 6319 )

TEST_CASE("Creating bit buffer with fixed size", "[BitBuffer]")
{
	SECTION("Valid (positive) capacity")
	{
		int validCapacity = GENERATE(1, 2, 3, 8, 16);
		BitBuffer bitBuffer(validCapacity);
		CHECK(bitBuffer.capacity == validCapacity);
	}
	SECTION("Initial size is 0")
	{
		int validCapacity = GENERATE(1, 2, 3, 8, 16);
		BitBuffer bitBuffer(validCapacity);
		CHECK(bitBuffer.size() == 0);
	}
	SECTION("Invalid (negative or zero) capacity")
	{
		auto invalidCapacity = GENERATE(0, -1, -2);
		REQUIRE_THROWS_AS(BitBuffer(invalidCapacity), std::invalid_argument);
	}
}

TEST_CASE("Write single bits to buffer", "[BitBuffer]")
{
	SECTION("Write bits as booleans")
	{
		BitBuffer buff{ 10 };
		buff.write(true);		// add bit '1' to the buffer
		CHECK(buff.size() == 1);
		buff.write(false);		// add bit '0' to the buffer
		CHECK(buff.size() == 2);
	}
	SECTION("Write bits as integers")
	{
		BitBuffer buff{ 10 };
		buff.write(0);			// add bit '0' to the buffer
		CHECK(buff.size() == 1);
		buff.write(1);			// add bit '1' to the buffer
		CHECK(buff.size() == 2);
	}
	SECTION("Write bits using shift operator")
	{
		BitBuffer buff{ 10 };
		buff << true << 0;
		CHECK(buff.size() == 2);
		buff << 1 << false;
		CHECK(buff.size() == 4);
	}
	SECTION("Write bits as invalid integers")
	{
		BitBuffer buff{ 10 };
		CHECK_THROWS_AS(buff.write(2), std::invalid_argument);
		CHECK_THROWS_AS(buff.write(-1), std::invalid_argument);
		CHECK(buff.size() == 0);
	}
	SECTION("Write bits when buffer is full")
	{
		BitBuffer buff{ 3 };
		buff.write(1);
		buff.write(0);
		buff.write(true);
		CHECK(buff.size() == 3); // buffer is full
		CHECK_THROWS_AS(buff.write(0), std::length_error); // cannot overflow the buffer
	}
}

BitBuffer getRandomBitBuffer(int capacity, int size)
{
	srand((int)time(NULL)); // it's not the best place to put it, but it makes no difference
	BitBuffer randomBuff{ capacity };
	for (int i = 0; i < size; i++) {
		bool randomBit = rand() % 2;
		randomBuff.write(randomBit);
	}

	return randomBuff;
}

TEST_CASE("Read single bits from buffer", "[BitBuffer]")
{
	SECTION("Read bit")
	{
		BitBuffer buff{ 10 };
		buff.write(0);
		buff.write(1);
		CHECK(buff.capacity == 10);
		CHECK(buff.size() == 2);
		
		bool bit0 = buff.read();
		CHECK(bit0 == false);
		CHECK(buff.size() == 1);

		bool bit1 = buff.read();
		CHECK(bit1 == true);
		CHECK(buff.size() == 0);
	}

	SECTION("Read bit from empty buffer")
	{
		BitBuffer buff{ 10 };
		REQUIRE(buff.size() == 0);
		CHECK_THROWS_AS(buff.read(), std::length_error);
	}
}

TEST_CASE("Write byte to buffer", "[BitBuffer]")
{
	SECTION("Write single byte to the buffer")
	{
		unsigned char byte = GENERATE('\x00', '\x01', '\x4F', '\xAB', '\xFE', '\xFF');
		BitBuffer buff{ 10 };
		buff.writeByte(byte);
		CHECK(buff.size() == 8);
	}
	
	SECTION("Write byte and bits alternately")
	{
		BitBuffer buff{ 10 };
		buff.write(1);
		CHECK(buff.size() == 1);

		buff.writeByte('\x0F');
		CHECK(buff.size() == 8 + 1);

		buff.write(0);
		CHECK(buff.size() == 8 + 1 + 1);
	}

	SECTION("Write byte when buffer is full")
	{
		BitBuffer buff{ 8 };
		buff.write(1);
		CHECK(buff.size() == 1);

		CHECK_THROWS_AS(buff.writeByte('\x00'), std::length_error);
	}

	SECTION("Write byte and read bit")
	{
		BitBuffer buff{ 10 };
		buff.writeByte('\x02'); // 0000 0010 <-- insert order: 0 -> 1 -> 0 -> ...

		bool bit0 = buff.read();
		CHECK(bit0 == false);

		bool bit1 = buff.read();
		CHECK(bit1 == true);

		for (int i = 0; i < 6; i++)	// remaining 6 bits
		{
			bit0 = buff.read();
			CHECK(bit0 == false);
		}
	}
}

TEST_CASE("Read byte from buffer")
{
	SECTION("Read single byte")
	{
		BitBuffer buff{ 10 };

		buff.writeByte(u'\x01');
		//buff.write(1);
		byte_t byte = buff.readByte();
		CHECK(byte == '\x01');
		//CHECK(buff.size() == 0);
	}

	SECTION("Read byte when not enough bits")
	{
		BitBuffer buff{ 10 };
		CHECK_THROWS_AS(buff.readByte(), std::length_error);

		buff.write(1);
		CHECK_THROWS_AS(buff.readByte(), std::length_error);
	}
}

TEST_CASE("Read all remaining bits")
{
	BitBuffer buff{ 10 };

	buff.write(1);
	buff.write(1);
	buff.write(0);
	buff.write(1);

	auto bits = buff.readAllBits();

	CHECK_THAT(bits, Catch::Equals(std::vector<bool>{ 1, 1, 0, 1 }));
}

TEST_CASE("Read all remaining bits as bytes")
{
	SECTION("Less than a byte")
	{
		BitBuffer buff{ 10 };
		buff.write(1);
		buff.write(0);
		buff.write(1);

		auto bytes = buff.readAllBytes(0);
		REQUIRE(bytes.size() == 1);
		CHECK(bytes[0] == '\x05');
	}
	SECTION("Byte and some")
	{
		BitBuffer buff{ 10 };
		buff.writeByte('\xFF');
		buff.write(1);

		auto bytes = buff.readAllBytes(0);
		CHECK_THAT(bytes, Catch::Equals(std::vector<byte_t>{ u'\xFF', u'\x01' }));
	}
}

TEST_CASE("Read all bits / bytes from empty buffer")
{
	BitBuffer buff{ 10 };
	
	auto bits = buff.readAllBits();
	CHECK(bits.size() == 0);

	auto bytes = buff.readAllBytes();
	CHECK(bytes.size() == 0);
}