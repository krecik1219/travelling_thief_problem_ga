#pragma once

#include <vector>

#include "City.hpp"
#include "TtpConfig.hpp"

namespace ttp {

class TspInstance
{
public:
	TspInstance(std::vector<City>&& cities);

	template <class RandomGenerator>
	static TspInstance createRandom(const TtpConfig& ttpConfig, RandomGenerator&& g);

private:
	std::vector<ttp::City> cityChain;
};

template <class RandomGenerator>
TspInstance ttp::TspInstance::createRandom(const TtpConfig& ttpConfig, RandomGenerator&& g)
{
	std::vector<City> cities;
	cities.reserve(ttpConfig.cities.size());
	std::transform(
		std::cbegin(ttpConfig.cities),
		std::cend(ttpConfig.cities),
		std::back_inserter(cities),
		[&](const auto& pair) {return pair.second; });
	std::shuffle(cities.begin(), cities.end(), g);
	return TspInstance(std::move(cities));
}

} // namespace ttp
