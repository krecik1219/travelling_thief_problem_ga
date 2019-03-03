#include "Knapsack.hpp"

#include <algorithm>
#include <numeric>

namespace ttp {

Knapsack::Knapsack(const uint32_t capacity)
	: capacity(capacity)
{
}

const ItemsPerCity & Knapsack::getItemsPerCity() const
{
	return itemsPerCity;
}

uint32_t Knapsack::getWeightForCity(const uint32_t cityId) const
{
	const auto& items = itemsPerCity.at(cityId);
	return std::accumulate(items.cbegin(), items.cend(), 0, [&items](const auto& item) {return item.weight; });
}

void Knapsack::fillKnapsack(const TtpConfig& ttpConfig)
{
	// based on profit to weight ratio
	auto allItems = ttpConfig.items;
	std::sort(allItems.begin(), allItems.end(),
		[](const auto& lhs, const auto& rhs)
		{
			return lhs.profit / lhs.weight < rhs.profit / rhs.weight;
		});
	for (auto i = 0u; i < allItems.size() && currentWeight < capacity; i++)
	{
		auto& item = allItems[i];
		if (currentWeight + item.weight <= capacity)
		{
			currentWeight += item.weight;
			itemsPerCity[item.cityId].push_back(std::move(item));
		}
	}
}

uint32_t Knapsack::getKnapsackValue() const
{
	return std::accumulate(itemsPerCity.cbegin(), itemsPerCity.cend(), 0,
		[](const auto& pair)
		{
			return std::accumulate(pair.second.cbegin(), pair.second.cend(), 0,
				[](const auto& item) {return item.profit; });
		}
	);
}

} // namespace ttp
