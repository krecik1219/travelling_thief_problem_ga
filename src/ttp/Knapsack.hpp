#pragma once

#include <cstdint>
#include <unordered_map>

#include "Item.hpp"
#include <configuration/TtpConfig.hpp>

namespace ttp {

using ItemsPerCity = std::unordered_map<uint32_t, std::vector<Item>>;

class Knapsack
{
public:
	explicit Knapsack(const uint32_t capacity);

	Knapsack() = delete;
	Knapsack(const Knapsack&) = default;
	Knapsack(Knapsack&&) = default;
	~Knapsack() = default;
	
	Knapsack& operator=(const Knapsack&) = delete;
	Knapsack& operator=(Knapsack&&) = delete;

	const ItemsPerCity& getItemsPerCity() const;
	uint32_t getWeightForCity(const uint32_t cityId) const;
	void clear();
	void addItem(const Item& item);
	uint32_t getKnapsackValue() const;
	uint32_t getKnapsackCapacity() const;
	uint32_t getCurrentWeight() const;
	std::string getStringRepresentation() const;

private:
	const uint32_t capacity;
	uint32_t currentWeight;
	ItemsPerCity itemsPerCity;
	uint32_t knapsackValue;
};

} // namespace ttp
