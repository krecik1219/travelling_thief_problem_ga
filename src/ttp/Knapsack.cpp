#include "Knapsack.hpp"

#include <algorithm>
#include <numeric>

namespace ttp {

Knapsack::Knapsack(const uint32_t capacity)
	: capacity(capacity)
	, currentWeight(0u)
	, knapsackValue(0u)
{
}

const ItemsPerCity& Knapsack::getItemsPerCity() const
{
	return itemsPerCity;
}

uint32_t Knapsack::getWeightForCity(const uint32_t cityId) const
{
	if (itemsPerCity.find(cityId) == itemsPerCity.end())
		return 0u;
	const auto& items = itemsPerCity.at(cityId);
	return std::accumulate(items.cbegin(), items.cend(), 0, [](const auto& acc, const auto& item) {return acc + item.weight; });
}

void Knapsack::clear()
{
	itemsPerCity.clear();
	currentWeight = 0u;
	knapsackValue = 0u;
}

void Knapsack::addItem(const Item& item)
{
	itemsPerCity[item.cityId].push_back(item);
	currentWeight += item.weight;
	knapsackValue += item.profit;
}

int32_t Knapsack::getKnapsackValue() const
{
	return static_cast<int32_t>(knapsackValue);
}

uint32_t Knapsack::getKnapsackCapacity() const
{
	return capacity;
}

uint32_t Knapsack::getCurrentWeight() const
{
	return currentWeight;
}

std::string Knapsack::getStringRepresentation() const
{
	std::string result;
	const std::string resultDelimiter = " ; ";
	const std::string subResultDelimiter = ", ";
	for (const auto& itemsInCity : itemsPerCity)
	{
		result.append(std::to_string(itemsInCity.first)).append(": [");
		std::string subResult;
		for (const auto& item : itemsInCity.second)
			subResult.append(std::to_string(item.index)).append(subResultDelimiter);
		if (!subResult.empty())
			subResult = subResult.substr(0, subResult.length() - subResultDelimiter.length());
		result.append(subResult).append("]").append(resultDelimiter);
	}
	if (!result.empty())
		result = result.substr(0, result.length() - resultDelimiter.length());
	result += "; total profit: " + std::to_string(knapsackValue) + "; total weight: " + std::to_string(currentWeight);
	return result;
}

} // namespace ttp
