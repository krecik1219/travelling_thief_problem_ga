#include "SolutionsSet.hpp"

#include <algorithm>
#include <unordered_set>
#include <limits>

namespace pareto {

SolutionsSet::SolutionsSet(const uint32_t populationSize)
{
	solutions.reserve(populationSize * 2);
}

SolutionsSet::~SolutionsSet() = default;

void SolutionsSet::setSolutions(std::vector<IndividualPtr>&& newSolutions)
{
	solutions = std::move(newSolutions);
}

void SolutionsSet::addSolution(IndividualPtr&& solution)
{
	solutions.push_back(std::move(solution));
}

const std::vector<ttp::TtpIndividual*>& SolutionsSet::getParetoFront() const
{
	return nonDominatedSets.at(0);
}

std::vector<IndividualPtr>& SolutionsSet::getSolutions()
{
	return solutions;
}

const std::vector<IndividualPtr>& SolutionsSet::getSolutions() const
{
	return solutions;
}

void SolutionsSet::addSolutions(std::vector<IndividualPtr>&& solutionsToAdd)
{
	solutions.reserve(solutions.size() + solutionsToAdd.size());
	solutions.insert(solutions.end(), std::make_move_iterator(solutionsToAdd.begin()), std::make_move_iterator(solutionsToAdd.end()));
	solutionsToAdd.erase(solutionsToAdd.begin(), solutionsToAdd.end());
}

void SolutionsSet::truncate(const uint32_t size)
{
	uint32_t indexOfLastIncludedNonDominatedSet = 0u;
	std::unordered_set<ttp::TtpIndividual*> keptSolutions;
	keptSolutions.reserve(size);
	for (auto i = 0; i < nonDominatedSets.size(); i++)
	{
		indexOfLastIncludedNonDominatedSet = i;
		if (keptSolutions.size() + nonDominatedSets[i].size() <= size)
		{
			keptSolutions.insert(nonDominatedSets[i].begin(), nonDominatedSets[i].end());
			if (keptSolutions.size() == size)
				break;
		}
		else
		{
			std::stable_sort(nonDominatedSets[i].begin(), nonDominatedSets[i].end(),
				[](const auto& lhs, const auto& rhs) {
				return lhs->getCrowdingDistance() > rhs->getCrowdingDistance();
				}
			);
			int32_t diff = static_cast<int32_t>(keptSolutions.size() + nonDominatedSets[i].size()) - static_cast<int32_t>(size);
			keptSolutions.insert(nonDominatedSets[i].begin(), std::prev(nonDominatedSets[i].end(), diff));
			nonDominatedSets[i].erase(std::prev(nonDominatedSets[i].end(), diff), nonDominatedSets[i].end()); // truncate non-dominated set
			break;
		}
	}
	// remove non-dominated sets which are not kept in solutions any more
	if (nonDominatedSets.size() > 1)
	{
		nonDominatedSets.erase(std::next(nonDominatedSets.begin(), indexOfLastIncludedNonDominatedSet + 1), nonDominatedSets.end());
	}
	
	// truncate main solutions vector
	solutions.erase(
		std::remove_if(solutions.begin(), solutions.end(),
			[&keptSolutions](const auto& s) {return keptSolutions.find(s.get()) == keptSolutions.end(); }),
		solutions.end()
	);
}

void SolutionsSet::nonDominatedSorting()
{
	if (solutions.size() == 0u)
		return;

	std::vector<ttp::TtpIndividual*> superSet;
	superSet.reserve(solutions.size());
	for (auto& solution : solutions)
	{
		solution->setCrowdingDistance(0);  // reset crowding distance
		solution->setRank(0);  // reset rank
		superSet.push_back(solution.get());
	}

	nonDominatedSets = std::vector<std::vector<ttp::TtpIndividual*>>();
	uint32_t rank = 1u;
	auto minMaxTimeObjective = 
		std::minmax_element(solutions.cbegin(), solutions.cend(),
			[](const auto& lhs, const auto& rhs) {
				return lhs->getCurrentTimeObjectiveFitness() < rhs->getCurrentTimeObjectiveFitness(); }
	);
	double timeObjectiveMaxMinDiff =
		(*minMaxTimeObjective.second)->getCurrentTimeObjectiveFitness() - (*minMaxTimeObjective.first)->getCurrentTimeObjectiveFitness();

	auto minMaxMinusProfitObjective =
		std::minmax_element(solutions.cbegin(), solutions.cend(),
			[](const auto& lhs, const auto& rhs) {
		return lhs->getCurrentMinusProfitObjectiveFitness() < rhs->getCurrentMinusProfitObjectiveFitness(); }
	);

	double minusProfitObjectiveMaxMinDiff =
		(*minMaxMinusProfitObjective.second)->getCurrentMinusProfitObjectiveFitness() - (*minMaxMinusProfitObjective.first)->getCurrentMinusProfitObjectiveFitness();

	while (!superSet.empty())
	{
		extractSingleParetoFront(superSet, rank);
		computeCrowdingDistances(nonDominatedSets.back(), timeObjectiveMaxMinDiff, minusProfitObjectiveMaxMinDiff);
		rank++;
	}
}

void SolutionsSet::extractSingleParetoFront(std::vector<ttp::TtpIndividual*>& superSet, const uint32_t rank)
{
	std::stable_sort(superSet.begin(), superSet.end(),
		[](const auto& lhs, const auto& rhs) {
		return lhs->getCurrentTimeObjectiveFitness() < rhs->getCurrentTimeObjectiveFitness(); }
	);

	nonDominatedSets.emplace_back();
	nonDominatedSets.back().push_back(superSet.at(0));
	superSet[0]->setRank(rank);

	std::unordered_set<ttp::TtpIndividual*> toBeRemovedFromNonDominatedSet;
	auto removePred = [&toBeRemovedFromNonDominatedSet](const auto& ttpIndividual) {
		return toBeRemovedFromNonDominatedSet.find(ttpIndividual) != toBeRemovedFromNonDominatedSet.end();
	};

	for (auto itSolutions = std::next(superSet.begin()); itSolutions != superSet.end(); itSolutions++)
	{
		toBeRemovedFromNonDominatedSet.clear();
		bool isSolutionDominated = false;
		for (auto itFront = nonDominatedSets.back().rbegin(); itFront != nonDominatedSets.back().rend() && !isSolutionDominated; itFront++)
		{
			if (doesFirstDominatesSecond(**itFront, **itSolutions))
				isSolutionDominated = true;
			else if (doesFirstDominatesSecond(**itSolutions, **itFront))
				toBeRemovedFromNonDominatedSet.insert(*itFront);
		}
		nonDominatedSets.back().erase(
			std::remove_if(nonDominatedSets.back().begin(), nonDominatedSets.back().end(), removePred),
			nonDominatedSets.back().end()
		);
		if (!isSolutionDominated)
		{
			nonDominatedSets.back().push_back(*itSolutions);
			(*itSolutions)->setRank(rank);
			(*itSolutions)->setCrowdingDistance(0);
		}
	}

	superSet.erase(
		std::remove_if(superSet.begin(), superSet.end(),
			[this](const auto& s) {
				return std::find(nonDominatedSets.back().begin(), nonDominatedSets.back().end(), s) != nonDominatedSets.back().end();
			}
		),
		superSet.end()
	);
}

void SolutionsSet::computeCrowdingDistances(std::vector<ttp::TtpIndividual*>& paretoFront,
	const double timeObjectiveMaxMinDiff, const double minusProfitObjectiveMaxMinDiff)
{
	paretoFront[0]->setCrowdingDistance(std::numeric_limits<double>::infinity());
	if (paretoFront.size() > 1)
		paretoFront[paretoFront.size() - 1]->setCrowdingDistance(std::numeric_limits<double>::infinity());

	if (paretoFront.size() < 3)
		return;

	// don't need to sort front in order of time objective, cause it is already sorted
	for (auto i = 1; i < paretoFront.size() - 1; i++)
	{
		double crowdingDistance = paretoFront[i]->getCrowdingDistance();
		crowdingDistance +=
			(paretoFront[i + 1]->getCurrentTimeObjectiveFitness() - paretoFront[i - 1]->getCurrentTimeObjectiveFitness()) / timeObjectiveMaxMinDiff;
		paretoFront[i]->setCrowdingDistance(crowdingDistance);
	}

	// minus profit objective
	std::stable_sort(paretoFront.begin(), paretoFront.end(),
		[](const auto& lhs, const auto& rhs) {
		return lhs->getCurrentMinusProfitObjectiveFitness() < rhs->getCurrentMinusProfitObjectiveFitness(); }
	);

	paretoFront[0]->setCrowdingDistance(std::numeric_limits<double>::infinity());
	paretoFront[paretoFront.size() - 1]->setCrowdingDistance(std::numeric_limits<double>::infinity());

	for (auto i = 1; i < paretoFront.size() - 1; i++)
	{
		double crowdingDistance = paretoFront[i]->getCrowdingDistance();
		crowdingDistance +=
			(paretoFront[i + 1]->getCurrentMinusProfitObjectiveFitness() - paretoFront[i - 1]->getCurrentMinusProfitObjectiveFitness()) / minusProfitObjectiveMaxMinDiff;
		paretoFront[i]->setCrowdingDistance(crowdingDistance);
	}
}

bool SolutionsSet::doesFirstDominatesSecond(const ttp::TtpIndividual& first, const ttp::TtpIndividual& second)
{
	return (first.getCurrentTimeObjectiveFitness() <= second.getCurrentTimeObjectiveFitness() &&
		first.getCurrentMinusProfitObjectiveFitness() <= second.getCurrentMinusProfitObjectiveFitness()) &&
		(first.getCurrentTimeObjectiveFitness() < second.getCurrentTimeObjectiveFitness() ||
			first.getCurrentMinusProfitObjectiveFitness() < second.getCurrentMinusProfitObjectiveFitness());
}

} // namespace pareto
