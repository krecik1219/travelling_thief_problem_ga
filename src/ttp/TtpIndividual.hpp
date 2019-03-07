#pragma once

#include "TspSolution.hpp"

namespace ttp {

using KnapsackPtr = std::shared_ptr<Knapsack>;

class TtpIndividual
{
public:

	TtpIndividual(const TtpConfig& ttpConfig, TspSolution&& tsp, KnapsackPtr knapsack);

	template <class RandomGenerator>
	static TtpIndividual createRandom(const TtpConfig& ttpConfig, KnapsackPtr knapsack, RandomGenerator&& g);

	float getFitness();
	void mutation();
	TtpIndividual crossover(const TtpIndividual& parent2);

private:
	float computeFitness() const;
	float computeAndSetFitness();

	const TtpConfig& ttpConfig;
	TspSolution tsp;
	KnapsackPtr knapsack;
	float currentFitness;
	bool isCurrentFitnessValid;
};

template <class RandomGenerator>
TtpIndividual TtpIndividual::createRandom(const TtpConfig& ttpConfig, KnapsackPtr knapsack, RandomGenerator&& g)
{
	return TtpIndividual(ttpConfig, TspSolution::createRandom(ttpConfig, std::forward<RandomGenerator>(g)), std::move(knapsack));
}

} // namespace ttp
