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
	uint32_t tournamentsNum;
	uint32_t maxPopulationsNum;  // 0 indicates no populations num limit
	std::chrono::seconds maxGAlgDuration;  // 0s indicates no time limit
};

template <class Individual>
class GAlg
{
public:

	void run();

private:

	using IndividualPtr = std::unique_ptr<Individual>;

	void initialize();
	void evaluate();
	void gaLoop();
	void selection();
	void crossover();
	void mutation();
	bool checkStopConditions();

	bool timeStopCondition();
	bool populationsNumStopCondition();

	GAlgParams params;
	std::function<IndividualPtr(void)> createRandomFun;

	std::vector<IndividualPtr> population;
	std::vector<IndividualPtr> selectedParents;
	IndividualPtr bestIndividual;

	Tp startTimestamp;
	uint32_t populatiosnNum;
};

template<class Individual>
void GAlg<Individual>::run()
{
	startTimestamp = SteadyClock::now();
	initialize();
	evaluate();
	gaLoop();
}

template<class Individual>
void GAlg<Individual>::initialize()
{
	population.reserve(params.populationSize);
	selectedParents.reserve(params.tournamentsNum);
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
	}
}

template<class Individual>
void GAlg<Individual>::selection()
{
	// tournament
	std::vector<Individual*> tournamentBatch;
	tournamentBatch.reserve(params.tournamentSize);
	auto& random = utils::rnd::Random::getInstance();
	for (auto i = 0u; i < params.tournamentsNum; i++)
	{
		for (auto j = 0u; j < params.tournamentSize; j++)
		{
			auto individualIndex = random.getRandomInt(0, params.populationSize - 1);
			tournamentBatch.push_back(population[individualIndex].get());
		}
		auto tournamentWinnerIt =
			std::max_element(tournamentBatch.cbegin(), tournamentBatch.cend(),
				[](const auto& lhs, const auto& rhs) {lhs->getCurrentFitness() < rhs->getCurrentFitness(); });
		selectedParents.push_back(std::make_unique(**tournamentWinnerIt));
		tournamentBatch.clear();
	}
	population.clear();
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

} // namespace ga
