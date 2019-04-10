#pragma once
#include <iostream>
#include <algorithm>
#include <chrono>
#include <functional>
#include <memory>
#include <numeric>
#include <vector>

#include <utils/RandomUtils.hpp>
#include <logger/Logger.hpp>
#include <configuration/GAlgConfig.hpp>
#include "selection/SelectionStrategy.hpp"
#include "selection/TournamentStrategy.hpp"
#include "selection/RouletteWheelStrategy.hpp"


namespace ga {

using SteadyClock = std::chrono::steady_clock;
using Tp = std::chrono::time_point<SteadyClock>;

template <class Individual>
class GAlg
{
public:
	using IndividualPtr = std::unique_ptr<Individual>;

	GAlg(const config::GAlgParams& params, std::function<IndividualPtr(void)> createRandomFun, logging::Logger& logger);

	GAlg() = delete;
	GAlg(const GAlg&) = delete;
	GAlg(GAlg&&) = delete;
	~GAlg() = default;

	GAlg& operator=(const GAlg&) = delete;
	GAlg& operator=(GAlg&&) = delete;

	void run();
	IndividualPtr getBestIndividual() const;
	std::vector<IndividualPtr> getPopulation() const;

private:

	void initialize();
	void evaluate();
	void gaLoop();
	void selection();
	void insertToNextPopulation(const Individual& parent1, const Individual& parent2, std::vector<IndividualPtr>& nextPopulation);
	void proceedWithOneParentInsertion(const Individual& parent1, const Individual& parent2, std::vector<IndividualPtr>& nextPopulation);
	void proceedWithBothParentsInsertion(const Individual& parent1, const Individual& parent2, std::vector<IndividualPtr>& nextPopulation);
	void followWithMutation(Individual& individual);
	bool checkStopConditions();
	std::unique_ptr<SelectionStrategy<Individual>> makeSelectionStrategy() const;

	bool timeStopCondition();
	bool populationsNumStopCondition();
	void setBestIndividualSoFar();

	void logState() const;

	config::GAlgParams params;
	std::function<IndividualPtr(void)> createRandomFun;

	std::unique_ptr<SelectionStrategy<Individual>> selectionStrategy;

	std::vector<IndividualPtr> population;
	IndividualPtr bestIndividualSoFar;
	logging::Logger& logger;
	Tp startTimestamp;
	uint32_t populationsNum;
};

template<class Individual>
GAlg<Individual>::GAlg(const config::GAlgParams& params, std::function<IndividualPtr(void)> createRandomFun, logging::Logger& logger)
	: params(params)
	, createRandomFun(std::move(createRandomFun))
	, selectionStrategy(makeSelectionStrategy())
	, logger(logger)
	, populationsNum(0)
{
	population.reserve(params.populationSize);
}

template<class Individual>
void GAlg<Individual>::run()
{
	startTimestamp = SteadyClock::now();
	initialize();
	evaluate();
	setBestIndividualSoFar();
	logState();
	gaLoop();
}

template<class Individual>
typename GAlg<Individual>::IndividualPtr GAlg<Individual>::getBestIndividual() const
{
	return std::make_unique<Individual>(*bestIndividualSoFar);
}

template<class Individual>
std::vector<typename GAlg<Individual>::IndividualPtr> GAlg<Individual>::getPopulation() const
{
	std::vector<IndividualPtr> populationCopy;
	populationCopy.reserve(population.size());
	for (const auto& individual : population)
		populationCopy.push_back(std::make_unique<Individual>(*individual));
	return populationCopy;
}

template<class Individual>
void GAlg<Individual>::initialize()
{
	for (auto i = 0u; i < params.populationSize; i++)
		population.push_back(createRandomFun());
}

template<class Individual>
void GAlg<Individual>::evaluate()
{
	std::for_each(population.begin(), population.end(), [](auto& individual) {individual->evaluate(); });
}

template<class Individual>
void GAlg<Individual>::gaLoop()
{
	while (!checkStopConditions())
	{
		selection();
		evaluate();
		populationsNum++;
		setBestIndividualSoFar();
		logState();
	}
}

template<class Individual>
void GAlg<Individual>::selection()
{
	std::vector<IndividualPtr> nextPopulation;
	nextPopulation.reserve(params.populationSize);
	while(nextPopulation.size() != params.populationSize)
	{
		const Individual& parent1 = selectionStrategy->selectParent(population);
		const Individual& parent2 = selectionStrategy->selectParent(population);
		insertToNextPopulation(parent1, parent2, nextPopulation);
	}
	population = std::move(nextPopulation);
}

template<class Individual>
void GAlg<Individual>::insertToNextPopulation(const Individual& parent1, const Individual& parent2, std::vector<IndividualPtr>& nextPopulation)
{
	auto& random = utils::rnd::Random::getInstance();
	auto crossoverRnd = random.getRandomDouble(0.0, 1.0);
	if (crossoverRnd <= params.crossoverProb)
	{
		auto offspring = parent1.crossoverNrx(parent2);
		followWithMutation(*offspring);
		nextPopulation.push_back(std::move(offspring));
	}
	else
	{
		if (nextPopulation.size() == params.populationSize - 1)
			proceedWithOneParentInsertion(parent1, parent2, nextPopulation);
		else
			proceedWithBothParentsInsertion(parent1, parent2, nextPopulation);
	}

	//auto& random = utils::rnd::Random::getInstance();
	//auto crossoverRnd = random.getRandomDouble(0.0, 1.0);
	//if (crossoverRnd <= params.crossoverProb)
	//{
	//	auto [offspring1, offspring2] = parent1.crossoverPmx(parent2);
	//	followWithMutation(*offspring1);
	//	nextPopulation.push_back(std::move(offspring1));
	//	followWithMutation(*offspring2);
	//	nextPopulation.push_back(std::move(offspring2));
	//}
	//else
	//{
	//	proceedWithBothParentsInsertion(parent1, parent2, nextPopulation);
	//}
}

template<class Individual>
void GAlg<Individual>::proceedWithOneParentInsertion(
	const Individual& parent1, const Individual& parent2, std::vector<IndividualPtr>& nextPopulation)
{
	auto& random = utils::rnd::Random::getInstance();
	auto rndVal = random.getRandomDouble(0.0, 1.0);
	std::unique_ptr<Individual> individual = nullptr;
	if(rndVal < 0.5)
		individual = std::make_unique<Individual>(parent1);
	else
		individual = std::make_unique<Individual>(parent2);
	followWithMutation(*individual);
	nextPopulation.push_back(std::move(individual));
}

template<class Individual>
void GAlg<Individual>::proceedWithBothParentsInsertion(
	const Individual & parent1, const Individual & parent2, std::vector<IndividualPtr>& nextPopulation)
{
	auto individual1 = std::make_unique<Individual>(parent1);
	auto individual2 = std::make_unique<Individual>(parent2);
	followWithMutation(*individual1);
	followWithMutation(*individual2);
	nextPopulation.push_back(std::move(individual1));
	nextPopulation.push_back(std::move(individual2));
}

template<class Individual>
void GAlg<Individual>::followWithMutation(Individual& individual)
{
	auto& random = utils::rnd::Random::getInstance();
	auto mutationRnd = random.getRandomDouble(0.0, 1.0);
	if (mutationRnd <= params.mutationProb)
		individual.mutation();
}

template<class Individual>
bool GAlg<Individual>::checkStopConditions()
{
	return populationsNumStopCondition() || timeStopCondition();
}

template<class Individual>
std::unique_ptr<SelectionStrategy<Individual>> GAlg<Individual>::makeSelectionStrategy() const
{
	if (params.selectionStrategy == "tournament")
		return std::make_unique<TournamentStrategy<Individual>>(params.tournamentSize);
	else if (params.selectionStrategy == "roulette")
		return std::make_unique<RouletteWheelStrategy<Individual>>();
	else
		throw std::runtime_error("Provided selection strategy name: " + params.selectionStrategy + " not matching any available strategy");
}

template<class Individual>
bool GAlg<Individual>::timeStopCondition()
{
	if (params.maxGAlgDuration == 0s)
		return false;
	return SteadyClock::now() - startTimestamp >= params.maxGAlgDuration;
}

template<class Individual>
bool GAlg<Individual>::populationsNumStopCondition()
{
	if (params.maxPopulationsNum == 0)
		return false;
	return populationsNum >= params.maxPopulationsNum;
}

template<class Individual>
void GAlg<Individual>::setBestIndividualSoFar()
{
	auto bestIndividualIt =
		std::max_element(population.cbegin(), population.cend(),
			[](const auto& lhs, const auto& rhs) {return lhs->getCurrentFitness() < rhs->getCurrentFitness(); });
	if (bestIndividualSoFar == nullptr)
	{
		bestIndividualSoFar = std::make_unique<Individual>(**bestIndividualIt);
	}
	else
	{
		const auto best = std::max((*bestIndividualIt).get(), bestIndividualSoFar.get(),
			[](const auto& lhs, const auto& rhs) {return lhs->getCurrentFitness() < rhs->getCurrentFitness(); });
		if (best != bestIndividualSoFar.get())
			bestIndividualSoFar = std::make_unique<Individual>(*best);
	}
}

template<class Individual>
void GAlg<Individual>::logState() const
{
	auto bestWorstIterators = std::minmax_element(population.cbegin(), population.cend(),
		[](const auto& lhs, const auto& rhs) {return lhs->getCurrentFitness() < rhs->getCurrentFitness(); });
	auto bestCurrentFitness = (*bestWorstIterators.second)->getCurrentFitness();
	auto worstCurrentFitness = (*bestWorstIterators.first)->getCurrentFitness();
	double sumOfFitnesses = std::accumulate(population.cbegin(), population.cend(), 0.0,
		[](const auto& acc, const auto& individual) {return acc + individual->getCurrentFitness(); });
	auto avgFitness = sumOfFitnesses / population.size();
	logger.log("%d, %.4f, %.4f, %.4f", populationsNum, bestCurrentFitness, avgFitness, worstCurrentFitness);
	//std::cout << populationsNum << ", " << bestCurrentFitness << ", " << avgFitness << ", " << worstCurrentFitness << std::endl;
}

} // namespace ga
