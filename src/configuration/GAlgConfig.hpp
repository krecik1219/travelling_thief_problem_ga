#pragma once

#include <cstdint>
#include <chrono>
#include <string>

namespace config {

struct GAlgParams
{
	uint32_t populationSize;
	uint32_t tournamentSize;
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
};

} // namespace config
