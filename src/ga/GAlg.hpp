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

#include <async/GenericLambdaAsyncTask.hpp>
#include <async/GenericLambdaAsyncSubTask.hpp>
#include <async/ExecutionManager.hpp>

namespace ga {

using SteadyClock = std::chrono::steady_clock;
using Tp = std::chrono::time_point<SteadyClock>;
using namespace pareto;
using namespace async;

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
	void mutateDuplicates();
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

	void asyncMutation();

	config::GAlgParams params;
	std::function<IndividualPtr(void)> createRandomFun;

	std::unique_ptr<SelectionStrategy<Individual>> selectionStrategy;

	IndividualPtr bestIndividualSoFar;
	gecco::SolutionsLogger& solutionsLogger;
	Tp startTimestamp;
	uint32_t populationsNum;
	SolutionsSet solutionsSet;

	std::vector<std::pair<Individual*, bool>> mutationAwaiting;
	
	std::atomic<uint32_t> mutationAwaitingNumWas;
	std::atomic<uint32_t> doneMutations;
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
	// std::for_each(solutions.begin(), solutions.end(), [](auto& individual) {individual->evaluate(); });

	AsyncTaskPtr task =
		std::make_unique<GenericLambdaAsyncTask>(
			[&solutions](const uint32_t /*sugesstedNumberOfSubTasks*/)
			{
				const uint32_t subTasksNum = 4u;
				const uint32_t singleBatchSize = static_cast<uint32_t>(solutions.size()) / subTasksNum;
				std::vector<AsyncSubTaskPtr> subTasks;
				for (auto i = 0u; i < subTasksNum + 1; i++)
				{
					subTasks.push_back(std::make_unique<GenericLambdaAsyncSubTask>(
						[&solutions, i, singleBatchSize]()
						{
							for (auto j = i * singleBatchSize; j < (i + 1) * singleBatchSize && j < solutions.size(); j++)
							{
								solutions[j]->evaluate();
							}
						}
					));
				}
				return subTasks;
			},
			4u
		);
	ExecutionManager executionManager(std::move(task));
	executionManager.executeTask();
	executionManager.waitForExecutionDone();
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
		std::cout << "Population num: " << populationsNum << std::endl;
		nonDominatedSorting();
		selection();
		evaluate();
		mutateDuplicates();
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
	asyncMutation();
	solutionsSet.addSolutions(std::move(children));
	mutationAwaiting.clear();
}

template<class Individual>
void GAlg<Individual>::mutateDuplicates()
{
	auto& solutions = solutionsSet.getSolutions();
	std::sort(solutions.begin(), solutions.end(),
		[](const auto& lhs, const auto& rhs)
		{
			return lhs->getCurrentMinusProfitObjectiveFitness() < rhs->getCurrentMinusProfitObjectiveFitness();
		}
	);
	std::stable_sort(solutions.begin(), solutions.end(),
		[](const auto& lhs, const auto& rhs)
		{
			return lhs->getCurrentTimeObjectiveFitness() < rhs->getCurrentTimeObjectiveFitness();
		}
	);
	for (auto i = 0u; i < solutions.size() - 1; i++)
	{
		if (std::abs(solutions[i]->getCurrentTimeObjectiveFitness() - solutions[i + 1]->getCurrentTimeObjectiveFitness()) < 0.0001 &&
			std::abs(solutions[i]->getCurrentMinusProfitObjectiveFitness() - solutions[i + 1]->getCurrentMinusProfitObjectiveFitness()) < 0.0001)
		{
			solutions[i]->mutation();
		}
	}
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
		// followWithMutation(*offspring);
		mutationAwaiting.push_back(std::make_pair(offspring.get(), false));
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
	//followWithMutation(*individual);
	mutationAwaiting.push_back(std::make_pair(individual.get(), false));
	children.push_back(std::move(individual));
}

template<class Individual>
void GAlg<Individual>::proceedWithBothParentsInsertion(
	const Individual& parent1, const Individual& parent2, std::vector<IndividualPtr>& children)
{
	auto individual1 = std::make_unique<Individual>(parent1);
	auto individual2 = std::make_unique<Individual>(parent2);
	//followWithMutation(*individual1);
	//followWithMutation(*individual2);
	mutationAwaiting.push_back(std::make_pair(individual1.get(), false));
	mutationAwaiting.push_back(std::make_pair(individual2.get(), false));
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

template<class Individual>
void GAlg<Individual>::asyncMutation()
{
	mutationAwaitingNumWas = static_cast<uint32_t>(mutationAwaiting.size());
	AsyncTaskPtr task =
		std::make_unique<GenericLambdaAsyncTask>(
			[this] (const uint32_t /*sugesstedNumberOfSubTasks*/)
			{
				const uint32_t subTasksNum = 4u;
				const uint32_t singleBatchSize = static_cast<uint32_t>(mutationAwaiting.size()) / subTasksNum;
				std::vector<AsyncSubTaskPtr> subTasks;
				for (auto i = 0u; i < subTasksNum + 1; i++)
				{
					uint32_t start = singleBatchSize * i;
					uint32_t end = singleBatchSize * (i + 1);
					subTasks.push_back(std::make_unique<GenericLambdaAsyncSubTask>(
						[this, i, start, end] ()
						{
							for (auto j = start; j < end && j < mutationAwaiting.size(); j++)
							{
								followWithMutation(*mutationAwaiting[j].first);
								mutationAwaiting[j].second = true;
								doneMutations++;
							}
						}
					));
				}
				return subTasks;
			},
			4u
	);
	ExecutionManager executionManager(std::move(task));
	executionManager.executeTask();
	executionManager.waitForExecutionDone();
}

} // namespace ga
