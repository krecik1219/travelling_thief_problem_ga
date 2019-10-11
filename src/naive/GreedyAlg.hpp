#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#include <ttp/City.hpp>
#include <configuration/TtpConfig.hpp>
#include <utils/RandomUtils.hpp>
#include <ttp/TtpIndividual.hpp>
#include <ttp/TspSolution.hpp>

namespace naive {

template <class Individual>
class GreedyAlg
{
public:
	using IndividualPtr = std::unique_ptr<Individual>;

	GreedyAlg(const uint32_t repetitionsNum, const config::TtpConfig& ttpConfig);
	IndividualPtr executeAlg();

private:
	uint32_t findNearestCityFor(const uint32_t cityId, const std::vector<uint32_t>& alreadyVisited) const;

	const uint32_t repetitionsNum;
	const config::TtpConfig& ttpConfig;
};

template<class Individual>
GreedyAlg<Individual>::GreedyAlg(const uint32_t repetitionsNum, const config::TtpConfig& ttpConfig)
	: repetitionsNum(repetitionsNum)
	, ttpConfig(ttpConfig)
{
}

template<class Individual>
typename GreedyAlg<Individual>::IndividualPtr GreedyAlg<Individual>::executeAlg()
{
	IndividualPtr best;
	double bestFitness = -std::numeric_limits<double>::infinity();
	for (auto i = 0u; i < repetitionsNum; i++)
	{
		std::vector<ttp::City> cities;
		cities.reserve(ttpConfig.cities.size());
		std::vector<uint32_t> alreadyVisited;
		alreadyVisited.reserve(ttpConfig.cities.size());

		auto& random = utils::rnd::Random::getInstance();
		uint32_t rndStartCityIndex = random.getRandomUint(0, static_cast<uint32_t>(ttpConfig.cities.size() - 1));
		cities.push_back(ttpConfig.cities[rndStartCityIndex]);
		alreadyVisited.push_back(rndStartCityIndex + 1);
		for (auto j = 1u; j < ttpConfig.cities.size(); j++)
		{
			auto nearestCityId = findNearestCityFor(cities[j - 1].index, alreadyVisited);
			cities.push_back(ttpConfig.cities[nearestCityId - 1]);
			alreadyVisited.push_back(nearestCityId);
		}
		ttp::TspSolution tsp(ttpConfig, std::move(cities));
		IndividualPtr individual = std::make_unique<ttp::TtpIndividual>(ttpConfig, std::move(tsp));
		double fitness = individual->evaluate();
		if (fitness > bestFitness) {
			best = std::move(individual);
			bestFitness = fitness;
		}
	}
	return best;
}

template<class Individual>
uint32_t GreedyAlg<Individual>::findNearestCityFor(const uint32_t cityId, const std::vector<uint32_t>& alreadyVisited) const
{
	auto minDistance = std::numeric_limits<double>::infinity();
	uint32_t cityIndexInVec = cityId - 1;
	auto nearestIndex = 0u;
	for (auto j = 0u; j < ttpConfig.cities.size(); j++)
	{
		if (j == cityIndexInVec)
			continue;
		if (std::find(alreadyVisited.cbegin(), alreadyVisited.cend(), j + 1) != alreadyVisited.end())
			continue;
		auto distance = ttpConfig.cities[cityIndexInVec].getDistance(ttpConfig.cities[j]);
		if (distance < minDistance)
		{
			minDistance = distance;
			nearestIndex = j;
		}
	}
	return nearestIndex + 1;
}

} // namespace naive
