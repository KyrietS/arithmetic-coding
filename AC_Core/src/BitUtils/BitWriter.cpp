#include "BitWriter.hpp"

#include <filesystem>

BitWriter::BitWriter(std::string path)
	: m_bitBuffer(BIT_BUFFER_CAPACITY)
{
	if (std::filesystem::exists(path))
		throw std::runtime_error("File with this name already exists");

	m_fileStream.open(path, std::ios_base::binary);
	if (!m_fileStream.is_open())
		throw std::runtime_error("Cannot open file");
}

void BitWriter::write(bool bit)
{
	m_bitBuffer << bit;
	if (m_bitBuffer.size() >= 8)
	{
		byte_t byte = m_bitBuffer.readByte();
		m_fileStream << byte;
	}
}

void BitWriter::write(int bit)
{
	if (bit == 0)
		write(false);
	else if (bit == 1)
		write(true);
	else
		throw std::invalid_argument("bit value can only be 1 or 0");
}

BitWriter& BitWriter::operator<<(bool bit)
{
	write(bit);
	return *this;
}

BitWriter& BitWriter::operator<<(int bit)
{
	write(bit);
	return *this;
}

void BitWriter::flush()
{
	auto bytes = m_bitBuffer.readAllBytes(0);
	for (auto byte : bytes)
		m_fileStream << byte;

	m_fileStream.flush();
}

BitWriter::~BitWriter()
{
	flush();
}

