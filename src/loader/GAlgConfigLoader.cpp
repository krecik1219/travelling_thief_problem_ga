#include "GAlgConfigLoader.hpp"

#include <chrono>
#include <fstream>

#include "ConfigParsingException.hpp"
#include <utils/StringUtils.hpp>

namespace loader {

config::GAlgConfigBase loader::GAlgConfigLoader::loadGAlgConfig(const std::string& filePath) const
{
	std::ifstream fileHandle(filePath);
	if (!fileHandle.is_open())
		throw std::runtime_error("Could not read file: " + filePath);

	config::GAlgConfig gAlgConfig;
	std::string line;
	while (std::getline(fileHandle, line))
	{
		decideWhatToDoWithLine(line, gAlgConfig);
	}

	return config::GAlgConfigBase(std::move(gAlgConfig));
}

void GAlgConfigLoader::decideWhatToDoWithLine(const std::string& line, config::GAlgConfig& gAlgConfig) const
{
	if (line.find("INSTANCE CONFIG PATH:") != std::string::npos)
	{
		auto value = prepareValueToStore(line);
		gAlgConfig.instanceFilePath = value;
	}
	else if (line.find("RESULTS CSV FILE:") != std::string::npos)
	{
		auto value = prepareValueToStore(line);
		gAlgConfig.resultsCsvFile = value;
	}
	else if (line.find("BEST INDIVIDUAL RESULT FILE:") != std::string::npos)
	{
		auto value = prepareValueToStore(line);
		gAlgConfig.bestIndividualResultFile = value;
	}
	else if (line.find("PARETO FRONT SOLUTIONS FILE:") != std::string::npos)
	{
		auto value = prepareValueToStore(line);
		gAlgConfig.paretoFrontSolutionsFile = value;
	}
	else if (line.find("PARETO FRONT VALUES FILE:") != std::string::npos)
	{
		auto value = prepareValueToStore(line);
		gAlgConfig.paretoFrontValuesFile = value;
	}
	else if (line.find("ALL SOLUTIONS VALUES FILE:") != std::string::npos)
	{
		auto value = prepareValueToStore(line);
		gAlgConfig.allSolutionsValuesFile = value;
	}
	else if (line.find("FRONT SOLUTIONS SORTED VALUES FILE:") != std::string::npos)
	{
		auto value = prepareValueToStore(line);
		gAlgConfig.frontSolutionsSortedValuesFile = value;
	}
	else if (line.find("HYPERVOLUME VALUE FILE:") != std::string::npos)
	{
		auto value = prepareValueToStore(line);
		gAlgConfig.hypervolumeValueFile = value;
	}
	else if (line.find("BEST GREEDY ALG PATH:") != std::string::npos)
	{
		auto value = prepareValueToStore(line);
		gAlgConfig.bestGreedyAlgPath = value;
	}
	else if (line.find("BEST RANDOM ALG PATH:") != std::string::npos)
	{
		auto value = prepareValueToStore(line);
		gAlgConfig.bestRandomAlgPath = value;
	}
	else if (line.find("NAIVE REPETITIONS:") != std::string::npos)
	{
		auto value = prepareValueToStore(line);
		gAlgConfig.naiveRepetitions = std::stoi(value);
	}
	else if (line.find("POPULATION SIZE:") != std::string::npos)
	{
		auto value = prepareValueToStore(line);
		gAlgConfig.gAlgParams.populationSize = std::stoi(value);
	}
	else if (line.find("SELECTION STRATEGY:") != std::string::npos)
	{
		auto value = prepareValueToStore(line);
		gAlgConfig.gAlgParams.selectionStrategy = value;
	}
	else if (line.find("TOURNAMENT SIZE:") != std::string::npos)
	{
		auto value = prepareValueToStore(line);
		gAlgConfig.gAlgParams.tournamentSize = std::stoi(value);
	}
	else if (line.find("MAX GENERATIONS NUM:") != std::string::npos)
	{
		auto value = prepareValueToStore(line);
		gAlgConfig.gAlgParams.maxPopulationsNum = std::stoi(value);
	}
	else if (line.find("MAX ALG SECONDS DURATION:") != std::string::npos)
	{
		auto value = prepareValueToStore(line);
		gAlgConfig.gAlgParams.maxGAlgDuration = std::chrono::seconds(std::stoi(value));
	}
	else if (line.find("CROSSOVER PROBABILITY:") != std::string::npos)
	{
		auto value = prepareValueToStore(line);
		gAlgConfig.gAlgParams.crossoverProb = std::stod(value);
	}
	else if (line.find("MUTATION PROBABILITY:") != std::string::npos)
	{
		auto value = prepareValueToStore(line);
		gAlgConfig.gAlgParams.mutationProb = std::stod(value);
	}
}

std::string GAlgConfigLoader::prepareValueToStore(const std::string & s) const
{
	auto pos = s.find(':');
	if (pos == std::string::npos)
		throw ConfigParsingException("Bad file structure at line: \"" + s + "\"");
	auto value = s.substr(pos + 1, s.length() - pos - 1);
	utils::str::trim(value);
	return value;
}

} // namespace config
