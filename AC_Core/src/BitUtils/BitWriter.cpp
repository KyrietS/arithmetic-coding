#include "BitWriter.hpp"

#include <filesystem>

BitWriter::BitWriter(std::string path)
	: m_bitBuffer(BIT_BUFFER_CAPACITY), m_stats(256)
{
	if (std::filesystem::exists(path))
		throw std::runtime_error("File with this name already exists");

	m_fileStream.open(path, std::ios_base::binary);
	if (!m_fileStream.is_open())
		throw std::runtime_error("Cannot open file");
}

void BitWriter::write(bool bit)
{
	if (m_currentByte >= 0 && m_currentByte < m_stats.size())
		m_stats[m_currentByte].writeCounter++;

	m_bitBuffer << bit;
	if (m_bitBuffer.size() >= BIT_BUFFER_CAPACITY)
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

void BitWriter::writeN(bool bit, int n)
{
	while (n--) {
		write(bit);
	}
}

void BitWriter::writeN(int bit, int n)
{
	writeN(bit == 0 ? false : true, n);
}

void BitWriter::flush()
{
	auto bytes = m_bitBuffer.readAllBytes(0);
	for (auto byte : bytes)
		m_fileStream << byte;

	m_fileStream.flush();
}

void BitWriter::beginByte(int byte)
{
	if (byte >= 0 && byte < m_stats.size())
	{
		m_currentByte = byte;
		m_stats[m_currentByte].readCounter += 8;
	}
}

std::vector<BitStat> BitWriter::getStats()
{
	return m_stats;
}

BitWriter::~BitWriter()
{
	flush();
}

