#include "TtpIndividual.hpp"

namespace ttp {

TtpIndividual::TtpIndividual(const TtpConfig& ttpConfig, TspSolution&& tsp, KnapsackPtr knapsack)
	: ttpConfig(ttpConfig)
	, tsp(std::move(tsp))
	, knapsack(std::move(knapsack))
	, currentFitness()
	, isCurrentFitnessValid(false)
{
}

float TtpIndividual::getCurrentFitness() const
{
	return currentFitness;
}

float TtpIndividual::evaluate()
{
	if (isCurrentFitnessValid)
		return currentFitness;

	return computeAndSetFitness();
}

float TtpIndividual::computeFitness() const
{
	return knapsack->getKnapsackValue() - tsp.getTripTime(*knapsack);
}

float TtpIndividual::computeAndSetFitness()
{
	auto fitness = computeFitness();
	currentFitness = fitness;
	isCurrentFitnessValid = true;
	return fitness;
}

void TtpIndividual::mutation()
{
	tsp.mutation();
	isCurrentFitnessValid = false;
}

TtpIndividual TtpIndividual::crossover(const TtpIndividual& parent2) const
{
	auto tripTime1 = static_cast<float>(knapsack->getKnapsackValue()) - currentFitness;
	auto tripTime2 = static_cast<float>(knapsack->getKnapsackValue()) - parent2.currentFitness;
	auto offspring = tsp.crossover(tripTime1, parent2.tsp, tripTime2);
	return TtpIndividual(ttpConfig, std::move(offspring), knapsack);
}

} // namespace ttp
