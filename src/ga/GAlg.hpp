#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <vector>

#include <utils/RandomUtils.hpp>


namespace ga {

using SteadyClock = std::chrono::steady_clock;
using Tp = std::chrono::time_point<SteadyClock>;

struct GAlgParams
{
	uint32_t populationSize;
	uint32_t tournamentSize;
	uint32_t maxPopulationsNum;  // 0 indicates no populations num limit
	std::chrono::seconds maxGAlgDuration;  // 0s indicates no time limit
	double crossoverProb;
	double mutationProb;
};

template <class Individual>
class GAlg
{
public:
	explicit GAlg(const GAlgParams& params);

	GAlg() = delete;
	GAlg(const GAlg&) = delete;
	GAlg(GAlg&&) = delete;
	~GAlg() = default;

	GAlg& operator=(const GAlg&) = delete;
	GAlg& operatoe=(GAlg&&) = delete;

	void run();

private:

	using IndividualPtr = std::unique_ptr<Individual>;

	void initialize();
	void evaluate();
	void gaLoop();
	void selection();
	Individual& selectParent();
	void insertToNextPopulation(const Individual& parent1, const Individual& parent2, std::vector<IndividualPtr>& nextPopulation);
	void followWithMutation(Individual& individual);
	bool checkStopConditions();

	bool timeStopCondition();
	bool populationsNumStopCondition();
	void setBestIndividualSoFar();

	GAlgParams params;
	std::function<IndividualPtr(void)> createRandomFun;

	std::vector<IndividualPtr> population;
	IndividualPtr bestIndividualSoFar;


	Tp startTimestamp;
	uint32_t populatiosnNum;
};

template<class Individual>
inline GAlg<Individual>::GAlg(const GAlgParams & params)
	: params(params)
	, populatiosnNum(0)
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
	gaLoop();
}

template<class Individual>
void GAlg<Individual>::initialize()
{
	for (auto i = 0u; i < params.populationSize; i++)
		population.push_back(createRandomFun());
}

template<class Individual>
inline void GAlg<Individual>::evaluate()
{
	std::for_each(popualtion.begin(), population.end(), [](auto& individual) {individual->evaluate(); });
}

template<class Individual>
void GAlg<Individual>::gaLoop()
{
	while (!checkStopConditions())
	{
		selection();
		crossover();
		mutation();
		evaluate();
		populationsNum++;
		setBestIndividualSoFar();
	}
}

template<class Individual>
void GAlg<Individual>::selection()
{
	std::vector<IndividualPtr> nextPopulation;
	nextPopulation.reserve(params.populationSize);
	auto& random = utils::rnd::Random::getInstance();
	while(nextPopulation.size() != params.populationSize)
	{
		const Individual& parent1 = selectParent();
		const Individual& parent2 = selectParent();
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
		auto offspring = parent1.crossover(parent2);
		followWithMutation(*offspring);
		nextPopulation.push_back(std::move(offspring));
	}
	else
	{
		auto individual1 = std::make_unique<Individual>(parent1);
		auto individual2 = std::make_unique<Individual>(parent2);
		followWithMutation(*individual1);
		followWithMutation(*individual2);
		nextPopulation.push_back(std::move(individual1));
		nextPopulation.push_back(std::move(individual2));
	}
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
Individual& GAlg<Individual>::selectParent()
{
	// tournament
	std::vector<Individual*> tournamentBatch;
	tournamentBatch.reserve(params.tournamentSize);
	for (auto j = 0u; j < params.tournamentSize; j++)
	{
		auto individualIndex = random.getRandomUint(0, params.populationSize - 1);
		tournamentBatch.push_back(population[individualIndex].get());
	}
	auto tournamentWinnerIt =
		std::max_element(tournamentBatch.cbegin(), tournamentBatch.cend(),
			[](const auto& lhs, const auto& rhs) {lhs->getCurrentFitness() < rhs->getCurrentFitness(); });
	return **tournamentWinnerIt;
}

template<class Individual>
bool GAlg<Individual>::checkStopConditions()
{
	return populationsNumStopCondition() || timeStopCondition();
}

template<class Individual>
bool GAlg<Individual>::timeStopCondition()
{
	return SteadyClock::now() - startTimestamp >= params.maxGAlgDuration;
}

template<class Individual>
bool GAlg<Individual>::populationsNumStopCondition()
{
	return populatiosnNum >= params.maxPopulationsNum;
}

template<class Individual>
void GAlg<Individual>::setBestIndividualSoFar()
{
	auto bestIndividualIt =
		std::max_element(tournamentBatch.cbegin(), tournamentBatch.cend(),
			[](const auto& lhs, const auto& rhs) {lhs->getCurrentFitness() < rhs->getCurrentFitness(); });
	bestIndividualSoFar = std::make_unique(**bestIndividualIt);
}

} // namespace ga
