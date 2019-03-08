#pragma once

#include <memory>

#include "TspSolution.hpp"

namespace ttp {

using KnapsackPtr = std::shared_ptr<Knapsack>;

class TtpIndividual
{
public:
	TtpIndividual() = delete;
	TtpIndividual(const TtpIndividual& other) = default;
	TtpIndividual(TtpIndividual&& other) = default;
	TtpIndividual(const TtpConfig& ttpConfig, TspSolution&& tsp, KnapsackPtr knapsack);

	TtpIndividual& operator=(const TtpIndividual&) = delete;
	TtpIndividual& operator=(TtpIndividual&&) = delete;

	template <class RandomGenerator>
	static std::unique_ptr<TtpIndividual> createRandom(const TtpConfig& ttpConfig, KnapsackPtr knapsack, RandomGenerator&& g);

	float getCurrentFitness() const;
	float evaluate();
	void mutation();
	TtpIndividual crossover(const TtpIndividual& parent2) const;

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
std::unique_ptr<TtpIndividual> TtpIndividual::createRandom(const TtpConfig& ttpConfig, KnapsackPtr knapsack, RandomGenerator&& g)
{
	return std::make_unique<TtpIndividual>(ttpConfig, TspSolution::createRandom(ttpConfig, std::forward<RandomGenerator>(g)), std::move(knapsack));
}

} // namespace ttp
