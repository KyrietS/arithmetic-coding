#pragma once
#include <string>

#include "Statistics.hpp"

class ArithmeticCoder
{
public:
	virtual Statistics encode(std::string path_in, std::string path_out) = 0;
	virtual void decode(std::string path_in, std::string path_out) = 0;
};
