#pragma once
#include <string>

class ArithmeticCoder
{
public:
	virtual void encode(std::string path_in, std::string path_out) = 0;
	virtual void decode(std::string path_in, std::string path_out) = 0;
};
