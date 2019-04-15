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
#include <pareto/SolutionsSet.hpp>
#include "selection/SelectionStrategy.hpp"
#include "selection/TournamentStrategy.hpp"
#include "selection/RouletteWheelStrategy.hpp"
#include <gecco/SolutionsLogger.hpp>


namespace ga {

using SteadyClock = std::chrono::steady_clock;
using Tp = std::chrono::time_point<SteadyClock>;
using namespace pareto;

// TODO: This approach is highly implementation dependent now, should be changed to generic one

template <class Individual>
class GAlg
{
public:
	using IndividualPtr = std::unique_ptr<Individual>;

	GAlg(const config::GAlgParams& params, std::function<IndividualPtr(void)> createRandomFun, gecco::SolutionsLogger& solutionsLogger);

	GAlg() = delete;
	GAlg(const GAlg&) = delete;
	GAlg(GAlg&&) = delete;
	~GAlg() = default;

	GAlg& operator=(const GAlg&) = delete;
	GAlg& operator=(GAlg&&) = delete;

	void run();
	IndividualPtr getBestIndividual() const;
	const SolutionsSet& getSolutionsSet() const;

private:

	void initialize();
	void evaluate();
	void nonDominatedSorting();
	void rankSorting();
	void gaLoop();
	void selection();
	void makeNextPopulation();
	void insertToChildren(const Individual& parent1, const Individual& parent2, std::vector<IndividualPtr>& children);
	void proceedWithOneParentInsertion(const Individual& parent1, const Individual& parent2, std::vector<IndividualPtr>& children);
	void proceedWithBothParentsInsertion(const Individual& parent1, const Individual& parent2, std::vector<IndividualPtr>& children);
	void followWithMutation(Individual& individual);
	bool checkStopConditions();
	std::unique_ptr<SelectionStrategy<Individual>> makeSelectionStrategy() const;

	bool timeStopCondition();
	bool populationsNumStopCondition();

	void logState() const;

	config::GAlgParams params;
	std::function<IndividualPtr(void)> createRandomFun;

	std::unique_ptr<SelectionStrategy<Individual>> selectionStrategy;

	IndividualPtr bestIndividualSoFar;
	gecco::SolutionsLogger& solutionsLogger;
	Tp startTimestamp;
	uint32_t populationsNum;
	SolutionsSet solutionsSet;
};

template<class Individual>
GAlg<Individual>::GAlg(const config::GAlgParams& params, std::function<IndividualPtr(void)> createRandomFun,
	gecco::SolutionsLogger& solutionsLogger)
	: params(params)
	, createRandomFun(std::move(createRandomFun))
	, selectionStrategy(makeSelectionStrategy())
	, solutionsLogger(solutionsLogger)
	, populationsNum(0)
	, solutionsSet(params.populationSize)
{
}

template<class Individual>
void GAlg<Individual>::run()
{
	startTimestamp = SteadyClock::now();
	initialize();
	evaluate();
	gaLoop();
	logState();
}

template<class Individual>
typename GAlg<Individual>::IndividualPtr GAlg<Individual>::getBestIndividual() const
{
	return std::make_unique<Individual>(*bestIndividualSoFar);
}

template<class Individual>
const SolutionsSet& GAlg<Individual>::getSolutionsSet() const
{
	return solutionsSet;
}

template<class Individual>
void GAlg<Individual>::initialize()
{
	for (auto i = 0u; i < params.populationSize; i++)
		solutionsSet.addSolution(createRandomFun());
}

template<class Individual>
void GAlg<Individual>::evaluate()
{
	auto& solutions = solutionsSet.getSolutions();
	std::for_each(solutions.begin(), solutions.end(), [](auto& individual) {individual->evaluate(); });
}

template<class Individual>
void GAlg<Individual>::nonDominatedSorting()
{
	solutionsSet.nonDominatedSorting();
}

template<class Individual>
void GAlg<Individual>::rankSorting()
{
	solutionsSet.rankSorting();
}

template<class Individual>
void GAlg<Individual>::gaLoop()
{
	while (!checkStopConditions())
	{
		nonDominatedSorting();
		selection();
		evaluate();
		rankSorting();
		makeNextPopulation();
		populationsNum++;
	}
}

template<class Individual>
void GAlg<Individual>::selection()
{
	std::vector<IndividualPtr> children;
	children.reserve(params.populationSize);
	while(children.size() != params.populationSize)
	{
		const Individual& parent1 = selectionStrategy->selectParent(solutionsSet.getSolutions());
		const Individual& parent2 = selectionStrategy->selectParent(solutionsSet.getSolutions());
		insertToChildren(parent1, parent2, children);
	}
	solutionsSet.addSolutions(std::move(children));
}

template<class Individual>
void GAlg<Individual>::makeNextPopulation()
{
	solutionsSet.truncate(params.populationSize);
}

template<class Individual>
void GAlg<Individual>::insertToChildren(const Individual& parent1, const Individual& parent2, std::vector<IndividualPtr>& children)
{
	auto& random = utils::rnd::Random::getInstance();
	auto crossoverRnd = random.getRandomDouble(0.0, 1.0);
	if (crossoverRnd <= params.crossoverProb)
	{
		auto offspring = parent1.crossoverNrx(parent2);
		followWithMutation(*offspring);
		children.push_back(std::move(offspring));
	}
	else
	{
		if (children.size() == params.populationSize - 1)
			proceedWithOneParentInsertion(parent1, parent2, children);
		else
			proceedWithBothParentsInsertion(parent1, parent2, children);
	}
}

template<class Individual>
void GAlg<Individual>::proceedWithOneParentInsertion(
	const Individual& parent1, const Individual& parent2, std::vector<IndividualPtr>& children)
{
	auto& random = utils::rnd::Random::getInstance();
	auto rndVal = random.getRandomDouble(0.0, 1.0);
	std::unique_ptr<Individual> individual = nullptr;
	if(rndVal < 0.5)
		individual = std::make_unique<Individual>(parent1);
	else
		individual = std::make_unique<Individual>(parent2);
	followWithMutation(*individual);
	children.push_back(std::move(individual));
}

template<class Individual>
void GAlg<Individual>::proceedWithBothParentsInsertion(
	const Individual& parent1, const Individual& parent2, std::vector<IndividualPtr>& children)
{
	auto individual1 = std::make_unique<Individual>(parent1);
	auto individual2 = std::make_unique<Individual>(parent2);
	followWithMutation(*individual1);
	followWithMutation(*individual2);
	children.push_back(std::move(individual1));
	children.push_back(std::move(individual2));
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
	//else if (params.selectionStrategy == "roulette")
	//	return std::make_unique<RouletteWheelStrategy<Individual>>();
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
void GAlg<Individual>::logState() const
{
	solutionsLogger.log(solutionsSet);
}

} // namespace ga
