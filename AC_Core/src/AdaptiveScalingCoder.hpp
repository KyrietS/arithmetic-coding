#pragma once
#include "ArithmeticCoder.hpp"
#include "AlphabetModel.hpp"

#include <string>

class AdaptiveScalingCoder : public ArithmeticCoder
{
public:
	void encode(std::string path_in, std::string path_out) override;
	void decode(std::string path_in, std::string path_out) override;

private:
	AlphabetModel model;
};
