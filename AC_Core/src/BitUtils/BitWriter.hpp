#include <fstream>
#include <string>
#include "BitBuffer.hpp"

class BitWriter
{
public:
	BitWriter(std::string path);

	void write(bool bit);
	void write(int bit);
	BitWriter& operator<< (bool bit);
	BitWriter& operator<< (int bit);

	void flush();

	virtual ~BitWriter();

private:
	const int BIT_BUFFER_CAPACITY = 8;

	std::ofstream m_fileStream;
	BitBuffer m_bitBuffer;
};