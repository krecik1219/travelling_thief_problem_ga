#include "TspSolution.hpp"

#include <algorithm>

namespace ttp {

TspSolution::TspSolution(std::vector<City>&& cities)
	: cityChain(std::move(cities))
{
}

float TspSolution::getTripTime(const Knapsack& knapsack, const TtpConfig& config) const
{
	if (cityChain.size() < 2)
		return 0;

	uint32_t totalWeight = 0u;
	float tripTime = 0;
	for (auto i = 0u; i < cityChain.size() - 1; i++)
	{
		auto distance = cityChain[i].getDistance(cityChain[i + 1]);
		totalWeight += knapsack.getWeightForCity(cityChain[i].index);
		auto velocity = getCurrentVelocity(distance, totalWeight, config);
		tripTime += distance / velocity;
	}
	// below finishing TSP cycle
	auto index = cityChain.size() - 1;
	auto distance = cityChain[index].getDistance(cityChain[0]);
	totalWeight += knapsack.getWeightForCity(cityChain[index].index);
	auto velocity = getCurrentVelocity(distance, totalWeight, config);
	tripTime += distance / velocity;
	return tripTime;
}

float TspSolution::getCurrentVelocity(const float distance, const uint32_t currentWeight, const TtpConfig& config) const
{
	return config.maxVelocity - currentWeight * ((config.maxVelocity - config.minVelocity) / config.capacityOfKnapsack);
}

} // namespace ttp
