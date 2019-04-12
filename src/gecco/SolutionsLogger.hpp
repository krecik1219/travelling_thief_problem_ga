#pragma once

#include <string>
#include <logger/Logger.hpp>
#include <pareto/SolutionsSet.hpp>

namespace gecco {

using namespace logging;

class SolutionsLogger
{
public:
	SolutionsLogger(const std::string& individualsLogPath, const std::string& objectiveFunctionsValuesPath);
	~SolutionsLogger();

	SolutionsLogger(const SolutionsLogger&) = delete;
	SolutionsLogger& operator=(const SolutionsLogger&) = delete;
	SolutionsLogger(SolutionsLogger&&) = delete;
	SolutionsLogger& operator=(SolutionsLogger&&) = delete;

	void log(const pareto::SolutionsSet& solutionsSet);

private:
	Logger individualsLog;
	Logger objectiveFunctionsValuesLog;
};

} // namespace gecco
