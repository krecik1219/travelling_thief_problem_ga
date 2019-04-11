#pragma once

#include <algorithm>
#include <cstdint>
#include <vector>
#include <memory>
#include <numeric>

#include "SelectionStrategy.hpp"
#include <utils/RandomUtils.hpp>

namespace ga {

// TODO: currently commented out, because of incompatibility with NSGA-II aproach

//template <class Individual>
//class RouletteWheelStrategy : public SelectionStrategy<Individual>
//{
//public:
//	using IndividualPtr = typename SelectionStrategy<Individual>::IndividualPtr;
//	virtual const Individual& selectParent(const std::vector<IndividualPtr>& population) const override;
//};
//
//template<class Individual>
//const Individual& RouletteWheelStrategy<Individual>::selectParent(const std::vector<IndividualPtr>& population) const
//{
//	std::vector<double> fitnesses(population.size());
//	std::generate(fitnesses.begin(), fitnesses.end(),
//		[i = 0u, &population]() mutable {return population[i++]->getCurrentFitness(); });
//	const double lowestFitness = *std::min_element(fitnesses.cbegin(), fitnesses.cend());
//	if (lowestFitness < 0)  // normalize all fitnesses to be positive by adding 1.1 * lowestFitness
//	{
//		std::for_each(fitnesses.begin(), fitnesses.end(), [&lowestFitness](auto& fitness) {fitness += (1.1 * (-lowestFitness)); });
//	}
//	std::vector<double> partialFitnessSums(population.size());
//	std::partial_sum(fitnesses.cbegin(), fitnesses.cend(), partialFitnessSums.begin());
//	auto& random = utils::rnd::Random::getInstance();
//	auto randomVal = random.getRandomDouble(0.0, partialFitnessSums.back());
//	auto winnerIt = std::lower_bound(partialFitnessSums.cbegin(), partialFitnessSums.cend(), randomVal);
//	auto index = std::distance(partialFitnessSums.cbegin(), winnerIt);
//	return *population[index];
//}

} //namespace ga
