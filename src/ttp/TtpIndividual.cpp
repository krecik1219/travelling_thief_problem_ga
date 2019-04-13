#include "TtpIndividual.hpp"

#include <cmath>
#include <memory>
#include <utility>

namespace ttp {

TtpIndividual::TtpIndividual(const config::TtpConfig& ttpConfig, TspSolution&& tsp)
	: ttpConfig(ttpConfig)
	, tsp(std::move(tsp))
	, knapsack(ttpConfig.capacityOfKnapsack)
	, timeObjectiveFitness(std::numeric_limits<double>::infinity())
	, minusProfitObjectiveFitness(std::numeric_limits<double>::infinity())
	, isCurrentFitnessValid(false)
	, rank(0)
	, crowdingDistance(0)
{
}

TtpIndividual::TtpIndividual(const TtpIndividual&) = default;
TtpIndividual::TtpIndividual(TtpIndividual&&) = default;
TtpIndividual::~TtpIndividual() = default;

double TtpIndividual::getTripTime() const
{
	const auto& cityChain = tsp.getCityChain();
	if (cityChain.size() < 2)
		return 0;

	uint32_t totalWeight = 0u;
	double tripTime = 0;
	for (auto i = 0u; i < cityChain.size() - 1; i++)
	{
		// double distance = cityChain[i].getDistance(cityChain[i + 1]);
		double distance = std::ceil(cityChain[i].getDistance(cityChain[i + 1]));  // gecco uses Math.ceil in distance calculation
		totalWeight += knapsack.getWeightForCity(cityChain[i].index);
		double velocity = getCurrentVelocity(totalWeight);
		tripTime += distance / velocity;
	}
	// below finishing TSP cycle
	auto index = cityChain.size() - 1;
	// double distance = cityChain[index].getDistance(cityChain[0]);
	double distance = std::ceil(cityChain[index].getDistance(cityChain[0]));  // gecco uses Math.ceil in distance calculation
	totalWeight += knapsack.getWeightForCity(cityChain[index].index);
	double velocity = getCurrentVelocity(totalWeight);
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
		// double distance = cityChain[i].getDistance(cityChain[i + 1]);
		double distance = std::ceil(cityChain[i].getDistance(cityChain[i + 1]));
		tripTime += distance / velocity;
	}
	// below finishing TSP cycle
	auto index = cityChain.size() - 1;
	// double distance = cityChain[index].getDistance(cityChain[0]);
	double distance = std::ceil(cityChain[index].getDistance(cityChain[0]));
	tripTime += distance / velocity;
	return tripTime;
}

double TtpIndividual::getCurrentVelocity(const uint32_t currentWeight) const
{
	return ttpConfig.maxVelocity - currentWeight * ((ttpConfig.maxVelocity - ttpConfig.minVelocity) / ttpConfig.capacityOfKnapsack);
}

std::pair<double, double> TtpIndividual::getCurrentMultiObjectiveFitness() const
{
	return std::make_pair(timeObjectiveFitness, minusProfitObjectiveFitness);
}

std::pair<double, double> TtpIndividual::evaluate()
{
	if (isCurrentFitnessValid)
		return std::make_pair(timeObjectiveFitness, minusProfitObjectiveFitness);

	return computeAndSetFitness();
}

double TtpIndividual::getCurrentTimeObjectiveFitness() const
{
	return timeObjectiveFitness;
}

double TtpIndividual::getCurrentMinusProfitObjectiveFitness() const
{
	return minusProfitObjectiveFitness;
}

std::pair<double, double> TtpIndividual::computeFitness()
{
	fillKnapsack();
	return std::make_pair(getTripTime(), -knapsack.getKnapsackValue());
}

std::pair<double, double> TtpIndividual::computeAndSetFitness()
{
	auto fitnessPair = computeFitness();
	timeObjectiveFitness = fitnessPair.first;
	minusProfitObjectiveFitness = fitnessPair.second;
	isCurrentFitnessValid = true;
	return fitnessPair;
}

double TtpIndividual::getCurrentFitness() const
{
	return -minusProfitObjectiveFitness - timeObjectiveFitness;
}

void TtpIndividual::mutation()
{
	tsp.mutation();
	isCurrentFitnessValid = false;
}

std::unique_ptr<TtpIndividual> TtpIndividual::crossoverNrx(const TtpIndividual& parent2) const
{
	//auto tripTime1 = static_cast<double>(knapsack.getKnapsackValue()) - getCurrentFitness();
	//auto tripTime2 = static_cast<double>(knapsack.getKnapsackValue()) - getCurrentFitness();  // IT WAS WRONG!!!
	auto offspring = tsp.crossoverNrx(timeObjectiveFitness, parent2.tsp, parent2.timeObjectiveFitness);
	return std::make_unique<TtpIndividual>(ttpConfig, std::move(offspring));
}

//OffspringsPtrsPair TtpIndividual::crossoverPmx(const TtpIndividual& parent2) const
//{
//	auto [offspringTsp1, offspringTsp2] = tsp.crossoverPmx(parent2.tsp);
//	return std::make_pair(
//		std::make_unique<TtpIndividual>(ttpConfig, std::move(offspringTsp1)),
//		std::make_unique<TtpIndividual>(ttpConfig, std::move(offspringTsp2))
//	);
//}

std::string TtpIndividual::getStringRepresentation() const
{
	auto tspStr = tsp.getStringRepresentation();
	auto knapsackStr = knapsack.getStringRepresentation();
	return "TSP: " + tspStr + "\n" + "knapsack: " + knapsackStr +
		"\ntotal time: " + std::to_string(getTripTime()) + "\nfitness: " + std::to_string(getCurrentFitness());
}

std::pair<std::string, std::string> TtpIndividual::getGeccoStringRepresentation() const
{
	auto tspStr = tsp.getGeccoStringRepresentation();
	auto knapsackStr = knapsack.getGeccoStringRepresentation(ttpConfig.items);
	return std::make_pair(std::move(tspStr), std::move(knapsackStr));
}

void TtpIndividual::setRank(const uint32_t newRank)
{
	rank = newRank;
}

uint32_t TtpIndividual::getRank() const
{
	return rank;
}

void TtpIndividual::setCrowdingDistance(const double newCrodwingDistance)
{
	crowdingDistance = newCrodwingDistance;
}

double TtpIndividual::getCrowdingDistance() const
{
	return crowdingDistance;
}

bool TtpIndividual::operator<(const TtpIndividual& other) const
{
	if (rank < other.rank)
		return true;
	if (rank == other.rank)
	{
		if (crowdingDistance > other.crowdingDistance)
			return true;
		else
			return false;
	}
	return false;
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
		//auto timeFromCityWithOneItem = getTripTime(item.cityId, item.weight);
		double velocity =
			knapsack.getCurrentWeight() + item.weight >= knapsack.getKnapsackCapacity() ? ttpConfig.minVelocity : getCurrentVelocity(item.weight);
		auto timeFromCityWithOneItem = totalTravelingDistanceFromCityWihId[item.cityId] / velocity;
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
