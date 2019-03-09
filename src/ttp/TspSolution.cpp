#include "TspSolution.hpp"

#include <algorithm>
#include <iterator>

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
	const int32_t neighbourhoodThreshold = static_cast<int32_t>(cityChain.size()) / 4;
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

TspSolution TspSolution::crossover(const double parent1TotalTime, const TspSolution& parent2, const double parent2TotalTime) const
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
