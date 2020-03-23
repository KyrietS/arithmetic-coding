#pragma once
#include <string>
#include <fstream>
#include "BitBuffer.hpp"

class BitReader 
{
public:
	BitReader(std::string filename);
	
	bool read();
	BitReader& operator>> (bool & bit);

	inline bool eof() {
		return m_endOfFile && m_bitBuffer.size() == 0; 
	}
private:
	const int BIT_BUFFER_CAPACITY = 8 * 1024;
	bool m_endOfFile;
	std::ifstream m_fileStream;
	BitBuffer m_bitBuffer;

	void fetchData();
};