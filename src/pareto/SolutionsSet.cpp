#include "SolutionsSet.hpp"

#include <algorithm>
#include <unordered_set>

namespace pareto {

void SolutionsSet::setSolutions(std::vector<IndividualPtr>&& newSolutions)
{
	solutions = std::move(newSolutions);
}

const std::vector<ttp::TtpIndividual*>& SolutionsSet::getParetoFront() const
{
	return nonDominatedSets.at(0);
}

void SolutionsSet::nonDominatedSorting()
{
	if (solutions.size() == 0u)
		return;

	std::vector<ttp::TtpIndividual*> superSet;
	superSet.reserve(solutions.size());
	for (auto& solution : solutions)
		superSet.push_back(solution.get());

	nonDominatedSets = std::vector<std::vector<ttp::TtpIndividual*>>();
	while (!superSet.empty())
	{
		extractSingleParetoFront(superSet);
	}
}

void SolutionsSet::extractSingleParetoFront(std::vector<ttp::TtpIndividual*>& superSet)
{
	std::stable_sort(superSet.begin(), superSet.end(),
		[](const auto& lhs, const auto& rhs) {
		return lhs->getCurrentTimeObjectiveFitness() < rhs->getCurrentTimeObjectiveFitness(); }
	);

	nonDominatedSets.emplace_back();
	nonDominatedSets.back().push_back(superSet.at(0));

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
			nonDominatedSets.back().push_back(*itSolutions);
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

bool SolutionsSet::doesFirstDominatesSecond(const ttp::TtpIndividual& first, const ttp::TtpIndividual& second)
{
	return (first.getCurrentTimeObjectiveFitness() <= second.getCurrentTimeObjectiveFitness() &&
		first.getCurrentMinusProfitObjectiveFitness() <= second.getCurrentMinusProfitObjectiveFitness()) &&
		(first.getCurrentTimeObjectiveFitness() < second.getCurrentTimeObjectiveFitness() ||
			first.getCurrentMinusProfitObjectiveFitness() < second.getCurrentMinusProfitObjectiveFitness());
}

} // namespace pareto
