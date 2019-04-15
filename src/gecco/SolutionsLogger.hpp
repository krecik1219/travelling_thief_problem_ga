#pragma once

#include <string>
#include <logger/Logger.hpp>
#include <pareto/SolutionsSet.hpp>

namespace gecco {

using namespace logging;

class SolutionsLogger
{
public:
	SolutionsLogger(const std::string& individualsLogPath, const std::string& objectiveFunctionsValuesPath,
		const std::string& allSolutionsLogPath, const std::string& paretoFrontSolutionsLogPath, const uint32_t bufferSize = 50000u);
	~SolutionsLogger();

	SolutionsLogger(const SolutionsLogger&) = delete;
	SolutionsLogger& operator=(const SolutionsLogger&) = delete;
	SolutionsLogger(SolutionsLogger&&) = delete;
	SolutionsLogger& operator=(SolutionsLogger&&) = delete;

	void log(const pareto::SolutionsSet& solutionsSet);

private:
	const uint32_t loggerBuffer;
	Logger individualsLog;
	Logger objectiveFunctionsValuesLog;
	Logger allSolutionsLog;
	Logger paretoFrontSolutionsLog;
};

} // namespace gecco
