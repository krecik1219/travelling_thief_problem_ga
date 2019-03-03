#pragma once

#include <cstdint>
#include <unordered_map>

#include "Item.hpp"
#include "TtpConfig.hpp"

namespace ttp {

using ItemsPerCity = std::unordered_map<uint32_t, std::vector<Item>>;

class Knapsack
{
public:
	Knapsack(const uint32_t capacity);
	
	const ItemsPerCity& getItemsPerCity() const;
	uint32_t getWeightForCity(const uint32_t cityId) const;
	void fillKnapsack(const TtpConfig& ttpConfig);
	uint32_t getKnapsackValue() const;

private:
	const uint32_t capacity;
	uint32_t currentWeight;
	ItemsPerCity itemsPerCity;
};

} // namespace ttp
