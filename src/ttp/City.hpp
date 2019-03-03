#pragma once
#include <cstdint>
#include <math.h>

namespace ttp {

struct City
{
	float getDistance(const City& other)
	{
		return sqrt((other.x - x) * (other.x - x) + (other.y - y) * (other.y - y));
	}

	uint32_t index;
	float x;
	float y;
};
} // namespace ttp
