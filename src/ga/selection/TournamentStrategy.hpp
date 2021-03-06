#pragma once

#include <algorithm>
#include <cstdint>
#include <vector>
#include <memory>

#include "SelectionStrategy.hpp"
#include <utils/RandomUtils.hpp>

namespace ga {

template <class Individual>
class TournamentStrategy : public SelectionStrategy<Individual>
{
public:
	using IndividualPtr = typename SelectionStrategy<Individual>::IndividualPtr;
	TournamentStrategy(const uint32_t tournamentSize);
	virtual const Individual& selectParent(const std::vector<IndividualPtr>& population) const override;

private:
	const uint32_t tournamentSize;
};

template<class Individual>
TournamentStrategy<Individual>::TournamentStrategy(const uint32_t tournamentSize)
	: SelectionStrategy<Individual>()
	, tournamentSize(tournamentSize)
{
	if (tournamentSize == 0)
		throw std::runtime_error("Tournament size can't be 0");
}

template<class Individual>
const Individual& TournamentStrategy<Individual>::selectParent(const std::vector<IndividualPtr>& population) const
{
	// tournament
	auto& random = utils::rnd::Random::getInstance();
	//std::vector<Individual*> tournamentBatch;
	//tournamentBatch.reserve(tournamentSize);
	auto individualIndex = random.getRandomUint(0, static_cast<uint32_t>(population.size() - 1));
	Individual* winner = population[individualIndex].get();
	for (auto j = 0u; j < tournamentSize - 1; j++)
	{
		individualIndex = random.getRandomUint(0, static_cast<uint32_t>(population.size() - 1));
		Individual* tmp = population[individualIndex].get();
		if (tmp->getCurrentFitness() > winner->getCurrentFitness())
			winner = tmp;
		//tournamentBatch.push_back(population[individualIndex].get());
	}
	/*auto tournamentWinnerIt =
		std::min_element(tournamentBatch.cbegin(), tournamentBatch.cend(),
			[](const auto& lhs, const auto& rhs) {return lhs->getCurrentFitness() < rhs->getCurrentFitness(); });*/
	//return **tournamentWinnerIt;
	return *winner;
}

} //namespace ga
