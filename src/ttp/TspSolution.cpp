#include "TspSolution.hpp"

#include <algorithm>
#include <iterator>
#include <map>

#include <utils/RandomUtils.hpp>

namespace ttp {

TspSolution::TspSolution(const TtpConfig& ttpConfig, std::vector<City>&& cities)
	: ttpConfig(ttpConfig)
	, cityChain(std::move(cities))
{
}

float TspSolution::getTripTime(const Knapsack& knapsack) const
{
	if (cityChain.size() < 2)
		return 0;

	uint32_t totalWeight = 0u;
	float tripTime = 0;
	for (auto i = 0u; i < cityChain.size() - 1; i++)
	{
		auto distance = cityChain[i].getDistance(cityChain[i + 1]);
		totalWeight += knapsack.getWeightForCity(cityChain[i].index);
		auto velocity = getCurrentVelocity(totalWeight);
		tripTime += distance / velocity;
	}
	// below finishing TSP cycle
	auto index = cityChain.size() - 1;
	auto distance = cityChain[index].getDistance(cityChain[0]);
	totalWeight += knapsack.getWeightForCity(cityChain[index].index);
	auto velocity = getCurrentVelocity(totalWeight);
	tripTime += distance / velocity;
	return tripTime;
}

float TspSolution::getCurrentVelocity(const uint32_t currentWeight) const
{
	return ttpConfig.maxVelocity - currentWeight * ((ttpConfig.maxVelocity - ttpConfig.minVelocity) / ttpConfig.capacityOfKnapsack);
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
	auto first = random.getRandomInt(0, lastIndexInChain);
	auto second = random.getRandomInt(0, lastIndexInChain);
	auto firstIt = std::next(cityChain.begin(), std::min(first, second));
	auto secondIt = std::next(cityChain.begin(), std::max(first, second));
	std::reverse(firstIt, secondIt);
	auto randomGene = random.getRandomInt(0, lastIndexInChain);
	auto nearestId = ttpConfig.nearestDistanceLookup.at(cityChain[randomGene].index).first;
	auto nearestCityIndex = static_cast<int32_t>(getIndexOfCityInChain(nearestId));
	auto leftBound = nearestCityIndex - neighbourhoodThreshold >= 0 ? nearestCityIndex - neighbourhoodThreshold : 0;
	auto rightBound =
		nearestCityIndex + neighbourhoodThreshold <= lastIndexInChain ?
			nearestCityIndex + neighbourhoodThreshold
			: lastIndexInChain;
	auto randomNeighbourIndex = random.getRandomInt(leftBound, rightBound);
	std::swap(cityChain[randomGene], cityChain[randomNeighbourIndex]);
}

TspSolution TspSolution::crossover(const float parent1TotalTime, const TspSolution& parent2, const float parent2TotalTime) const
{
	// NRX
	std::map<uint32_t, float> stepsSum;
	auto& random = utils::rnd::Random::getInstance();
	auto referenceCityPos = random.getRandomInt(0, static_cast<uint32_t>(cityChain.size() - 1));
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


} // namespace ttp
