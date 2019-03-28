#include "TspSolution.hpp"

#include <algorithm>
#include <iterator>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include <utils/RandomUtils.hpp>

namespace ttp {

TspSolution::TspSolution(const config::TtpConfig& ttpConfig, std::vector<City>&& cities)
	: ttpConfig(ttpConfig)
	, cityChain(std::move(cities))
{
}

const std::vector<ttp::City>& TspSolution::getCityChain() const
{
	return cityChain;
}

double TspSolution::getTotalDistance() const
{
	if (cityChain.size() < 2)
		return 0;

	double distance = 0;
	for (auto i = 0u; i < cityChain.size() - 1; i++)
		distance += cityChain[i].getDistance(cityChain[i + 1]);
	// below finishing TSP cycle
	auto index = cityChain.size() - 1;
	distance += cityChain[index].getDistance(cityChain[0]);
	return distance;
}

uint32_t TspSolution::getStepsNumTo(const uint32_t refCityId, const uint32_t destCityId) const
{
	auto stepsNum = 0u;
	auto refCityPos = getIndexOfCityInChain(refCityId);
	for (auto i = refCityPos; i < cityChain.size(); i++)
	{
		if (cityChain[i].index == destCityId)
			return stepsNum;
		stepsNum++;
	}
	for (auto i = 0u; i < refCityPos; i++)
	{
		if (cityChain[i].index == destCityId)
			return stepsNum;
		stepsNum++;
	}
	return 0u;
}

uint32_t TspSolution::getIndexOfCityInChain(const uint32_t cityId) const
{
	return static_cast<uint32_t>(
		std::distance(cityChain.cbegin(),
			std::find_if(cityChain.cbegin(), cityChain.cend(), [cityId](const auto& city) {return city.index == cityId; }))
		);
}

void TspSolution::mutation()
{
	// IRGIBNNM
	//const int32_t neighbourhoodThreshold = static_cast<int32_t>(cityChain.size()) / 100 + 1;
	const int32_t neighbourhoodThreshold = 5;
	auto& random = utils::rnd::Random::getInstance();
	auto lastIndexInChain = static_cast<int32_t>(cityChain.size() - 1);
	auto first = random.getRandomUint(0, lastIndexInChain);
	auto second = random.getRandomUint(0, lastIndexInChain);
	auto firstIt = std::next(cityChain.begin(), std::min(first, second));
	auto secondIt = std::next(cityChain.begin(), std::max(first, second));
	std::reverse(firstIt, secondIt);
	auto randomGene = random.getRandomUint(0, lastIndexInChain);
	auto nearestId = ttpConfig.nearestDistanceLookup.at(cityChain[randomGene].index).first;
	auto nearestCityIndex = static_cast<int32_t>(getIndexOfCityInChain(nearestId));
	auto leftBound = nearestCityIndex - neighbourhoodThreshold >= 0 ? nearestCityIndex - neighbourhoodThreshold : 0;
	auto rightBound =
		nearestCityIndex + neighbourhoodThreshold <= lastIndexInChain ?
			nearestCityIndex + neighbourhoodThreshold
			: lastIndexInChain;
	auto randomNeighbourIndex = random.getRandomUint(leftBound, rightBound);
	std::swap(cityChain[randomGene], cityChain[randomNeighbourIndex]);
}

TspSolution TspSolution::crossoverNrx(const double parent1TotalTime, const TspSolution& parent2, const double parent2TotalTime) const
{
	// NRX
	std::unordered_map<uint32_t, double> stepsSum;
	stepsSum.reserve(cityChain.size());
	auto& random = utils::rnd::Random::getInstance();
	auto referenceCityPos = random.getRandomUint(0, static_cast<uint32_t>(cityChain.size() - 1));
	auto referenceCityId = cityChain[referenceCityPos].index;
	for (auto i = 0u; i < cityChain.size(); i++)
		stepsSum[i + 1] = (getStepsNumTo(referenceCityId, i + 1) * parent1TotalTime + parent2.getStepsNumTo(referenceCityId, i + 1) * parent2TotalTime);
	
	auto offspring = ttpConfig.cities;
	for (auto i = 0u; i < cityChain.size(); i++)
	{
		for (auto j = i + 1; j < cityChain.size(); j++)
		{
			if (stepsSum[j + 1] < stepsSum[i + 1])
				std::swap(offspring[i], offspring[j]);
		}
	}
	return TspSolution(ttpConfig, std::move(offspring));
}

std::pair<TspSolution, TspSolution> TspSolution::crossoverPmx(const TspSolution& parent2) const
{
	auto& random = utils::rnd::Random::getInstance();
	uint32_t partitionIndex1 = random.getRandomUint(0, static_cast<uint32_t>(cityChain.size() - 1));
	uint32_t partitionIndex2 = partitionIndex1;
	while(partitionIndex2 == partitionIndex1)
		partitionIndex2 = random.getRandomUint(0, static_cast<uint32_t>(cityChain.size() - 1));
	if (partitionIndex2 < partitionIndex1)
		std::swap(partitionIndex1, partitionIndex2);
	
	return std::make_pair(
		TspSolution(ttpConfig, pmx(*this, parent2, partitionIndex1, partitionIndex2)),
		TspSolution(ttpConfig, pmx(parent2, *this, partitionIndex1, partitionIndex2))
	);
}

std::vector<City> TspSolution::pmx(const TspSolution& parent1,
	const TspSolution& parent2, const uint32_t partitionIndex1, const uint32_t partitionIndex2) const
{
	// use some extra memory to speed up lookups
	std::unordered_map<uint32_t, uint32_t> backCityIndexLookup;
	backCityIndexLookup.reserve(parent1.cityChain.size());
	for (auto i = 0u; i < parent1.cityChain.size(); i++)
		backCityIndexLookup[parent1.cityChain[i].index] = i;
	std::unordered_set<uint32_t> alreadyInOffspring;
	alreadyInOffspring.reserve(partitionIndex2 - partitionIndex1 + 1);
	std::vector<City> offspringCities(parent1.cityChain.size());

	// copy cities from random slice in parent1
	for (auto i = partitionIndex1; i < partitionIndex2; i++)
	{
		offspringCities[i] = parent1.cityChain[i];
		alreadyInOffspring.insert(offspringCities[i].index);
	}

	// first part to the left of slice
	for (auto i = 0u; i < partitionIndex1; i++)
	{
		uint32_t candidateId = parent2.cityChain[i].index;
		uint32_t indexOfCandidate = i;
		while (alreadyInOffspring.find(candidateId) != alreadyInOffspring.end())
		{
			indexOfCandidate = backCityIndexLookup[candidateId];
			candidateId = parent2.cityChain[indexOfCandidate].index;
		}
		offspringCities[i] = parent2.cityChain[indexOfCandidate];
	}

	// second part to the right of slice
	for (auto i = partitionIndex2; i < parent2.cityChain.size(); i++)
	{
		uint32_t candidateId = parent2.cityChain[i].index;
		uint32_t indexOfCandidate = i;
		while (alreadyInOffspring.find(candidateId) != alreadyInOffspring.end())
		{
			indexOfCandidate = backCityIndexLookup[candidateId];
			candidateId = parent2.cityChain[indexOfCandidate].index;
		}
		offspringCities[i] = parent2.cityChain[indexOfCandidate];
	}

	return offspringCities;
}

std::string TspSolution::getStringRepresentation() const
{
	std::string result;
	const std::string delimiter = " - ";
	for (const auto& city : cityChain)
		result.append(std::to_string(city.index)).append(delimiter);
	if (!result.empty())
		result = result.substr(0, result.length() - delimiter.length());
	result += " ; total distance: " + std::to_string(getTotalDistance());
	return result;
}

} // namespace ttp
