#pragma once

#include <cstdint>
#include <chrono>
#include <string>

namespace config {

struct GAlgParams
{
	uint32_t populationSize;
	std::string selectionStrategy;
	uint32_t tournamentSize;  // matters when selectionStrategy == "tournament"
	uint32_t maxPopulationsNum;  // 0 indicates no populations num limit
	std::chrono::seconds maxGAlgDuration;  // 0s indicates no time limit
	double crossoverProb;
	double mutationProb;
};

struct GAlgConfig
{
	GAlgParams gAlgParams;
	std::string instanceFilePath;
	std::string resultsCsvFile;
	std::string bestIndividualResultFile;
	std::string paretoFrontSolutionsFile;
	std::string paretoFrontValuesFile;
	std::string bestGreedyAlgPath;
	std::string bestRandomAlgPath;
	uint32_t naiveRepetitions;
};

} // namespace config
