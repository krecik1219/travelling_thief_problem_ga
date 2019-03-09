#include "TtpIndividual.hpp"

#include <memory>

namespace ttp {

TtpIndividual::TtpIndividual(const config::TtpConfig& ttpConfig, TspSolution&& tsp)
	: ttpConfig(ttpConfig)
	, tsp(std::move(tsp))
	, knapsack(ttpConfig.capacityOfKnapsack)
	, currentFitness(-std::numeric_limits<double>::infinity())
	, isCurrentFitnessValid(false)
{
}

double TtpIndividual::getTripTime() const
{
	const auto& cityChain = tsp.getCityChain();
	if (cityChain.size() < 2)
		return 0;

	uint32_t totalWeight = 0u;
	double tripTime = 0;
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

double TtpIndividual::getTripTime(const uint32_t startCityId, const uint32_t weight) const
{
	auto startCityPos = tsp.getIndexOfCityInChain(startCityId);
	const auto& cityChain = tsp.getCityChain();
	if (cityChain.size() < 2)
		return 0;

	double tripTime = 0;
	double velocity =
		knapsack.getCurrentWeight() + weight >= knapsack.getKnapsackCapacity() ? ttpConfig.minVelocity : getCurrentVelocity(weight);
	for (auto i = startCityPos; i < cityChain.size() - 1; i++)
	{
		auto distance = cityChain[i].getDistance(cityChain[i + 1]);
		tripTime += distance / velocity;
	}
	// below finishing TSP cycle
	auto index = cityChain.size() - 1;
	auto distance = cityChain[index].getDistance(cityChain[0]);
	tripTime += distance / velocity;
	return tripTime;
}

double TtpIndividual::getCurrentVelocity(const uint32_t currentWeight) const
{
	return ttpConfig.maxVelocity - currentWeight * ((ttpConfig.maxVelocity - ttpConfig.minVelocity) / ttpConfig.capacityOfKnapsack);
}

double TtpIndividual::getCurrentFitness() const
{
	return currentFitness;
}

double TtpIndividual::evaluate()
{
	if (isCurrentFitnessValid)
		return currentFitness;

	return computeAndSetFitness();
}

double TtpIndividual::computeFitness()
{
	fillKnapsack();
	return knapsack.getKnapsackValue() - getTripTime();
}

double TtpIndividual::computeAndSetFitness()
{
	auto fitness = computeFitness();
	currentFitness = fitness;
	isCurrentFitnessValid = true;
	return fitness;
}

void TtpIndividual::mutation()
{
	tsp.mutation();
	isCurrentFitnessValid = false;
}

std::unique_ptr<TtpIndividual> TtpIndividual::crossover(const TtpIndividual& parent2) const
{
	auto tripTime1 = static_cast<double>(knapsack.getKnapsackValue()) - currentFitness;
	auto tripTime2 = static_cast<double>(knapsack.getKnapsackValue()) - parent2.currentFitness;
	auto offspring = tsp.crossover(tripTime1, parent2.tsp, tripTime2);
	return std::make_unique<TtpIndividual>(ttpConfig, std::move(offspring));
}

std::string TtpIndividual::getStringRepresentation() const
{
	auto tspStr = tsp.getStringRepresentation();
	auto knapsackStr = knapsack.getStringRepresentation();
	return "TSP: " + tspStr + "\n" + "knapsack: " + knapsackStr +
		"\ntotal time: " + std::to_string(getTripTime()) + "\nfitness: " + std::to_string(currentFitness);
}

void TtpIndividual::fillKnapsack()
{
	knapsack.clear();
	std::unordered_map<uint32_t, double> scorePerItemWithId;
	scorePerItemWithId.reserve(ttpConfig.items.size());
	std::unordered_map<uint32_t, double> totalTravelingDistanceFromCityWihId;
	totalTravelingDistanceFromCityWihId.reserve(ttpConfig.cities.size());
	const auto& cities = tsp.getCityChain();
	totalTravelingDistanceFromCityWihId[cities[cities.size() - 1].index] = cities[cities.size() - 1].getDistance(cities[0]);
	auto citiesNum = static_cast<int>(cities.size());
	for (int i = citiesNum - 2; i >= 0; i--)
	{
		totalTravelingDistanceFromCityWihId[cities[i].index] =
			cities[i].getDistance(cities[i + 1]) + totalTravelingDistanceFromCityWihId[cities[i + 1].index];
	}
	double totalTravellingTimeWithoutItems = totalTravelingDistanceFromCityWihId[cities[0].index] / ttpConfig.maxVelocity;
	std::unordered_map<uint32_t, double> fitnessUtilization;
	fitnessUtilization.reserve(ttpConfig.items.size());
	for (const auto& item : ttpConfig.items)
	{
		auto timeFromCityWithOneItem = getTripTime(item.cityId, item.weight);
		auto totalTimeWithOneItem =
			(totalTravelingDistanceFromCityWihId[cities[0].index] - totalTravelingDistanceFromCityWihId[item.cityId]) / ttpConfig.maxVelocity +
			timeFromCityWithOneItem;
		scorePerItemWithId[item.index] = item.profit - timeFromCityWithOneItem;
		fitnessUtilization[item.index] = totalTravellingTimeWithoutItems + item.profit - totalTimeWithOneItem;
	}
	std::vector<Item> items = ttpConfig.items;
	std::sort(items.begin(), items.end(),
		[&scorePerItemWithId](const auto& lhs, const auto& rhs) {return scorePerItemWithId[lhs.index] > scorePerItemWithId[rhs.index]; });
	for (const auto& item : items)
	{
		if (knapsack.getCurrentWeight() + item.weight < knapsack.getKnapsackCapacity() && fitnessUtilization[item.index] > 0)
		{
			knapsack.addItem(item);
		}
		if (knapsack.getCurrentWeight() == knapsack.getKnapsackCapacity())
			break;
	}
}

} // namespace ttp
