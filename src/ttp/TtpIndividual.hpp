#pragma once

#include "TspSolution.hpp"

namespace ttp {

using KnapsackPtr = std::shared_ptr<Knapsack>;

class TtpIndividual
{
public:

	TtpIndividual(const TtpConfig& ttpConfig, TspSolution&& tsp, KnapsackPtr&& knapsack);

	template <class RandomGenerator>
	static TtpIndividual createRandom(const TtpConfig& ttpConfig, KnapsackPtr knapsack, RandomGenerator&& g);

	float computeFitness() const;
	void mutation();

private:
	const TtpConfig& ttpConfig;
	TspSolution tsp;
	KnapsackPtr knapsack;
};

template <class RandomGenerator>
TtpIndividual TtpIndividual::createRandom(const TtpConfig& ttpConfig, KnapsackPtr knapsack, RandomGenerator&& g)
{
	return TtpIndividual(ttpConfig, TspSolution::createRandom(ttpConfig, g), std::move(knapsack));
}

} // namespace ttp
