#pragma once
#include <math.h>

class RNG
{
public:
	RNG() = default;
	RNG(double lseed)
		:
		seed(lseed)
	{}
	~RNG() = default;

	double Random(double dec01, double dec02, double dec03)
	{
		double i = dec01 * dec01 + dec02 * dec02 + dec03 * dec03;
		double j = 3.2753 * dec01 + dec03 * 9.0293 + dec03 * 1.0289;
		i = sqrt(i); j = sqrt(j);
		return  sin(i * j * cos(seed));
	}

private:
	double seed;
};
