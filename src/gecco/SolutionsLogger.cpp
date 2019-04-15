#include "SolutionsLogger.hpp"

namespace gecco {


SolutionsLogger::SolutionsLogger(const std::string& individualsLogPath, const std::string& objectiveFunctionsValuesPath,
	const std::string& allSolutionsLogPath, const std::string& paretoFrontSolutionsLogPath, const uint32_t bufferSize)
	: loggerBuffer(bufferSize)
	, individualsLog(individualsLogPath, loggerBuffer)
	, objectiveFunctionsValuesLog(objectiveFunctionsValuesPath, loggerBuffer)
	, allSolutionsLog(allSolutionsLogPath, loggerBuffer)
	, paretoFrontSolutionsLog(paretoFrontSolutionsLogPath, loggerBuffer)
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
		objectiveFunctionsValuesLog.log("%.16f %.16f", solution->getCurrentTimeObjectiveFitness(), -solution->getCurrentMinusProfitObjectiveFitness());
	}
	for (auto i = 0u; i < solutions.size(); i++)
	{
		allSolutionsLog.log("%d, %.4f, %.4f", i + 1, solutions[i]->getCurrentTimeObjectiveFitness(), solutions[i]->getCurrentMinusProfitObjectiveFitness());
	}
	auto paretoFront = solutionsSet.getParetoFront();
	std::stable_sort(paretoFront.begin(), paretoFront.end(),
		[](const auto& lhs, const auto& rhs) {return lhs->getCurrentTimeObjectiveFitness() < rhs->getCurrentTimeObjectiveFitness(); });
	for (auto i = 0u; i < paretoFront.size(); i++)
	{
		paretoFrontSolutionsLog.log("%d, %.4f, %.4f", i + 1, paretoFront[i]->getCurrentTimeObjectiveFitness(), paretoFront[i]->getCurrentMinusProfitObjectiveFitness());
	}
}

} // namespace gecco
