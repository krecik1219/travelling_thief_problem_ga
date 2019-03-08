#pragma once

#include <vector>

#include "City.hpp"
#include "TtpConfig.hpp"
#include "Knapsack.hpp"

namespace ttp {

class TspSolution
{
public:
	TspSolution() = delete;
	TspSolution(const TspSolution& other) = default;
	TspSolution(TspSolution&& other) = default;
	TspSolution(const TtpConfig& ttpConfig, std::vector<City>&& cities);

	TspSolution& operator=(const TspSolution& other) = delete;
	TspSolution& operator=(TspSolution&& other) = delete;

	template <class RandomGenerator>
	static TspSolution createRandom(const TtpConfig& ttpConfig, RandomGenerator&& g);

	float getTripTime(const Knapsack& knapsack) const;
	float getCurrentVelocity(const uint32_t currentWeight) const;
	uint32_t getStepsNumTo(const uint32_t refCity, const uint32_t cityId) const;
	uint32_t getIndexOfCityInChain(const uint32_t cityId) const;
	void mutation();

	TspSolution crossover(const float parent1Fitness, const TspSolution& parent2, const float parent2Fitness) const;

private:
	const TtpConfig& ttpConfig;
	std::vector<ttp::City> cityChain;
};

template <class RandomGenerator>
TspSolution TspSolution::createRandom(const TtpConfig& ttpConfig, RandomGenerator&& g)
{
	auto cities = ttpConfig.cities;
	std::shuffle(cities.begin(), cities.end(), g);
	return TspSolution(ttpConfig, std::move(cities));
}

} // namespace ttp
