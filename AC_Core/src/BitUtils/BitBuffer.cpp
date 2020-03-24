#include "BitBuffer.hpp"
#include <stdexcept>
#include <string>

BitBuffer::BitBuffer(int buffCapacity)
	: capacity(buffCapacity)
{
	if (buffCapacity <= 0)
	{
		std::string msg = "capacity is not a positive value (" + std::to_string(buffCapacity) + ")";
		throw std::invalid_argument(msg);
	}

	m_bits.resize(capacity, false);	// create capacity-sized array with 0 as default value
	m_size = 0;
	m_readIndex = 0;
	m_writeIndex = 0;
}

size_t BitBuffer::size()
{
	return m_size;
}

void BitBuffer::write(bool bit)
{
	if (m_size == capacity)
		throw std::length_error("the buffer is full");

	m_bits[m_writeIndex] = bit;
	
	m_size++;
	m_writeIndex = (m_writeIndex + 1) % capacity;
}

void BitBuffer::write(int bit)
{
	if (bit == 0)
		write(false);
	else if (bit == 1)
		write(true);
	else
		throw std::invalid_argument("bit value can only be 1 or 0");
}

BitBuffer& BitBuffer::operator<<(bool bit)
{
	write(bit);
	return *this;
}

BitBuffer& BitBuffer::operator<<(int bit)
{
	write(bit);
	return *this;
}

void BitBuffer::writeByte(byte_t byte)
{
	if (m_size + 8 > capacity)
		throw std::length_error("Buffer has not enough space for another byte");
	
	for (int i = 0; i < 8; i++)
	{
		bool bit = byte & 0x01;
		write(bit);

		byte >>= 1;
	}
}

bool BitBuffer::read()
{
	if (m_size == 0)
		throw std::length_error("cannot read from empty buffer");

	bool bit = m_bits[m_readIndex];
	m_size--;
	m_readIndex = (m_readIndex + 1) % capacity;

	return bit;
}

BitBuffer& BitBuffer::operator>> (bool& bit)
{
	bit = read();
	return *this;
}

byte_t BitBuffer::readByte()
{
	if (m_size < 8)
		throw std::length_error("Buffer contains not enough bits for one byte");

	byte_t byte{ 0 };
	for (int i = 0; i < 8; i++)
	{
		bool bit = read();
		byte_t mask = 0x00 | bit;
		mask <<= i;

		byte = byte | mask;
	}

	return byte;
}

std::vector<bool> BitBuffer::readAllBits()
{
	size_t numOfBits = m_size;
	std::vector<bool> bits(numOfBits);

	for (int i = 0; i < numOfBits; i++)
	{
		bits[i] = read();
	}

	return bits;
}

std::vector<byte_t> BitBuffer::readAllBytes(bool padding)
{
	size_t numOfBytes = m_size / 8;
	bool remainder = (numOfBytes * 8 != m_size) && m_size != 0;
	std::vector<byte_t> bytes(numOfBytes + remainder);
	for (int i = 0; i < numOfBytes; i++)
	{
		bytes[i] = readByte();
	}

	if (remainder)
	{
		byte_t lastByte = padding ? 0xFF : 0x00;
		auto lastBits = readAllBits();
		for (int i = 0; i < lastBits.size(); i++)
		{
			bool bit = lastBits[i];
			if (bit) {
				byte_t mask = '\x01';
				mask <<= i;
				lastByte = lastByte | mask;
			}
			else {
				byte_t mask = '\x01';
				mask <<= i;
				mask = ~mask;
				lastByte = lastByte & mask;
			}
		}
		bytes[bytes.size() - 1] = lastByte;
	}

	return bytes;
}