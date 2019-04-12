#include "SolutionsLogger.hpp"

namespace gecco {


SolutionsLogger::SolutionsLogger(const std::string& individualsLogPath, const std::string& objectiveFunctionsValuesPath)
	: individualsLog(individualsLogPath, 50000u)
	, objectiveFunctionsValuesLog(objectiveFunctionsValuesPath, 50000u)
{
}

SolutionsLogger::~SolutionsLogger() = default;

void SolutionsLogger::log(const pareto::SolutionsSet& solutionsSet)
{
	const auto& solutions = solutionsSet.getSolutions();
	for (const auto& solution : solutions)
	{
		auto[tsp, knp] = solution->getGeccoStringRepresentation();
		individualsLog.log("%s", tsp.c_str());
		individualsLog.log("%s", knp.c_str());
		individualsLog.logNewLine();
		objectiveFunctionsValuesLog.log("%.16f, %.16f", solution->getCurrentTimeObjectiveFitness(), -solution->getCurrentMinusProfitObjectiveFitness());
	}
}

} // namespace gecco
