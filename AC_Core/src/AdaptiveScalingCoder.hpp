#pragma once
#include "ArithmeticCoder.hpp"
#include "Statistics.hpp"

#include <string>

class AdaptiveScalingCoder : public ArithmeticCoder
{
public:
	Statistics encode(std::string path_in, std::string path_out) override;
	void decode(std::string path_in, std::string path_out) override;
};
