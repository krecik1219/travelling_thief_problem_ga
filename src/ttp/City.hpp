#pragma once
#include <cstdint>
#include <math.h>

namespace ttp {

struct City
{
	double getDistance(const City& other) const
	{
		return sqrt((other.x - x) * (other.x - x) + (other.y - y) * (other.y - y));
	}

	uint32_t index;
	double x;
	double y;
};
} // namespace ttp
