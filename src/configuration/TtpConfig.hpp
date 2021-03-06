#pragma once

#include <algorithm>
#include <cstdint>
#include <unordered_map>
#include <string>
#include <vector>

#include <ttp/City.hpp>
#include <ttp/Item.hpp>

namespace config {

struct TtpConfig
{
	void fillNearestDistanceLookup()
	{
		auto minDistance = std::numeric_limits<double>::infinity();
		for (auto i = 0u; i < cities.size(); i++)
		{
			auto nearestIndex = 0u;
			for (auto j = 0u; j < cities.size(); j++)
			{
				if (j == i)
					continue;
				auto distance = cities[i].getDistance(cities[j]);
				if (distance < minDistance)
				{
					minDistance = distance;
					nearestIndex = j;
				}
			}
			nearestDistanceLookup[i + 1] = std::make_pair(nearestIndex + 1, minDistance);  // + 1 cause of cities numeration in config
			minDistance = std::numeric_limits<double>::infinity();
		}
	}

	std::string problemName;
	std::string knapsackDataType;
	uint32_t dimenssion;
	uint32_t itemsNum;
	uint32_t capacityOfKnapsack;
	double minVelocity;
	double maxVelocity;
	double rentingRatio;
	std::vector<ttp::City> cities;
	std::vector<ttp::Item> items;
	std::unordered_map<uint32_t, std::pair<uint32_t, double>> nearestDistanceLookup;
};
} // namespace config