#pragma once

#include <memory>
#include <vector>

#include <ttp/TtpIndividual.hpp>


namespace pareto {

using IndividualPtr = std::unique_ptr<ttp::TtpIndividual>;

class SolutionsSet
{
public:

	void setSolutions(std::vector<IndividualPtr>&& newSolutions);
	const std::vector<ttp::TtpIndividual*>& getParetoFront() const;

	void nonDominatedSorting();

private:
	void extractSingleParetoFront(std::vector<ttp::TtpIndividual*>& superSet);
	bool doesFirstDominatesSecond(const ttp::TtpIndividual& first, const ttp::TtpIndividual& second);

	std::vector<IndividualPtr> solutions;
	std::vector<std::vector<ttp::TtpIndividual*>> nonDominatedSets;
};

} // namespace pareto
