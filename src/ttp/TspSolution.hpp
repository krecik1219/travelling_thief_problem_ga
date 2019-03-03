#pragma once

#include <vector>

#include "City.hpp"
#include "TtpConfig.hpp"
#include "Knapsack.hpp"

namespace ttp {

class TspSolution
{
public:
	TspSolution(std::vector<City>&& cities);

	template <class RandomGenerator>
	static TspSolution createRandom(const TtpConfig& ttpConfig, RandomGenerator&& g);

	float getTripTime(const Knapsack& knapsack, const TtpConfig & config) const;
	float getCurrentVelocity(const float distance, const uint32_t currentWeight, const TtpConfig & config) const;

private:
	std::vector<ttp::City> cityChain;
};

template <class RandomGenerator>
TspSolution TspSolution::createRandom(const TtpConfig& ttpConfig, RandomGenerator&& g)
{
	std::vector<City> cities;
	cities.reserve(ttpConfig.cities.size());
	std::transform(
		std::cbegin(ttpConfig.cities),
		std::cend(ttpConfig.cities),
		std::back_inserter(cities),
		[&cities](const auto& pair) {return pair.second; });
	std::shuffle(cities.begin(), cities.end(), g);
	return TspSolution(std::move(cities));
}

} // namespace ttp
