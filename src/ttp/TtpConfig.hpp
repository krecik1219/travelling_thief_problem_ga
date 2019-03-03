#pragma once

#include <cstdint>
#include <map>
#include <string>

#include "City.hpp"
#include "Item.hpp"

namespace ttp {

struct TtpConfig
{
	std::string problemName;
	std::string knapsackDataType;
	uint32_t dimenssion;
	uint32_t itemsNum;
	uint32_t capacityOfKnapsack;
	float minSpeed;
	float maxSpeed;
	float rentingRatio;
	std::map<uint32_t, City> cities;
	std::map<uint32_t, Item> items;
};
} // namespace ttp