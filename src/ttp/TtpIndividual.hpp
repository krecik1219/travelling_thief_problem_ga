#pragma once

#include <memory>

#include "TspSolution.hpp"

namespace ttp {

class TtpIndividual;

using OffspringsPtrsPair = std::pair<std::unique_ptr<TtpIndividual>, std::unique_ptr<TtpIndividual>>;

class TtpIndividual
{
public:
	TtpIndividual(const config::TtpConfig& ttpConfig, TspSolution&& tsp);

	TtpIndividual() = delete;
	TtpIndividual(const TtpIndividual&) = default;
	TtpIndividual(TtpIndividual&&) = default;
	~TtpIndividual() = default;


	TtpIndividual& operator=(const TtpIndividual&) = delete;
	TtpIndividual& operator=(TtpIndividual&&) = delete;

	template <class RandomGenerator>
	static std::unique_ptr<TtpIndividual> createRandom(const config::TtpConfig& ttpConfig, RandomGenerator&& g);

	double getTripTime() const;
	double getTripTime(const uint32_t startCityPos, const uint32_t weight) const;
	double getCurrentVelocity(const uint32_t currentWeight) const;
	double getCurrentFitness() const;
	double evaluate();
	void mutation();
	std::unique_ptr<TtpIndividual> crossoverNrx(const TtpIndividual& parent2) const;
	OffspringsPtrsPair crossoverPmx(const TtpIndividual& parent2) const;
	std::string getStringRepresentation() const;

private:
	double computeFitness();
	double computeAndSetFitness();
	void fillKnapsack();

	const config::TtpConfig& ttpConfig;
	TspSolution tsp;
	Knapsack knapsack;
	double currentFitness;
	bool isCurrentFitnessValid;
};

template <class RandomGenerator>
std::unique_ptr<TtpIndividual> TtpIndividual::createRandom(const config::TtpConfig& ttpConfig, RandomGenerator&& g)
{
	return std::make_unique<TtpIndividual>(ttpConfig, TspSolution::createRandom(ttpConfig, std::forward<RandomGenerator>(g)));
}

} // namespace ttp
