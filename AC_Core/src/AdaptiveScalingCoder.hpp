#pragma once
#include "ArithmeticCoder.hpp"
#include "Statistics.hpp"

#include <string>

class AdaptiveScalingCoder : public ArithmeticCoder
{
public:
	AdaptiveScalingCoder(bool printProgress = false)
		: printProgress(printProgress) {}
	Statistics encode(std::string path_in, std::string path_out) override;
	void decode(std::string path_in, std::string path_out) override;

private:
	bool printProgress;
	int currentProgress = 0;
	const size_t progressbarWidth = 70;

	inline void updateProgress(double progress);
	inline void clearProgress();
};
