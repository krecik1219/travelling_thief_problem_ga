#pragma once

#include <memory>
#include <vector>

#include <ttp/TtpIndividual.hpp>


namespace pareto {

using IndividualPtr = std::unique_ptr<ttp::TtpIndividual>;

// TODO: optimization

class SolutionsSet
{
public:
	SolutionsSet(const uint32_t populationSize);

	~SolutionsSet();

	SolutionsSet() = delete;
	SolutionsSet(const SolutionsSet&) = delete;
	SolutionsSet& operator=(const SolutionsSet&) = delete;
	SolutionsSet(SolutionsSet&&) = delete;
	SolutionsSet& operator=(SolutionsSet&&) = delete;

	void setSolutions(std::vector<IndividualPtr>&& newSolutions);
	void addSolution(IndividualPtr&& solution);
	const std::vector<ttp::TtpIndividual*>& getParetoFront() const;
	std::vector<IndividualPtr>& getSolutions();
	const std::vector<IndividualPtr>& getSolutions() const;
	void addSolutions(std::vector<IndividualPtr>&& solutionsToAdd);
	void truncate(const uint32_t size);

	void nonDominatedSorting();
	void rankSorting();
	void crowdingDistanceSorting();
	std::pair<double, double> getMaxMinObjectiveFunctionsDiff() const;

private:
	void extractSingleParetoFront(std::vector<ttp::TtpIndividual*>& superSet, const uint32_t rank);
	void computeCrowdingDistances(std::vector<ttp::TtpIndividual*>& paretoFront,
		const double timeObjectiveMaxMinDiff, const double minusProfitObjectiveMaxMinDiff);
	bool doesFirstDominatesSecond(const ttp::TtpIndividual& first, const ttp::TtpIndividual& second);

	std::vector<IndividualPtr> solutions;
	std::vector<std::vector<ttp::TtpIndividual*>> nonDominatedSets;
};

} // namespace pareto
