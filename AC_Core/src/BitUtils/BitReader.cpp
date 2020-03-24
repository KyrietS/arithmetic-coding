#include "BitReader.hpp"

#include <filesystem>

BitReader::BitReader(std::string path)
	: m_bitBuffer(BIT_BUFFER_CAPACITY)
{
	if (!std::filesystem::exists(path))
		throw std::runtime_error("file does not exists");

	m_fileStream.open(path, std::istream::binary);
	if (!m_fileStream.is_open())
		throw std::runtime_error("Cannot open file");

	m_endOfFile = false;

	fetchData();
}

bool BitReader::read() 
{
	if (m_bitBuffer.size() == 0 && !m_endOfFile)	// buffer is empty
	{
		fetchData();	// fetch next block of bytes to the buffer
	}

	if (eof()) // when whole file is read, then return 0 bits
		return 0;
	else
		return m_bitBuffer.read();
}

BitReader& BitReader::operator>> (bool& bit)
{
	bit = read();
	return *this;
}

void BitReader::fetchData()
{
	size_t numOfBits = m_bitBuffer.capacity - m_bitBuffer.size();
	size_t numOfBytesToFetch = numOfBits / 8;
	
	for (size_t i = 0; i < numOfBytesToFetch; i++)
	{
		int byte = m_fileStream.get();
		if (byte == EOF)
		{
			m_endOfFile = true;
			return;
		}

		m_bitBuffer.writeByte(byte);
	}
}