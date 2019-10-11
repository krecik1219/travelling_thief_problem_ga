#pragma once

#include <cstdint>
#include <functional>
#include <memory>

#include <logger/Logger.hpp>

namespace naive {

template <class Individual>
class RandomSelectionAlg
{
public:
	using IndividualPtr = std::unique_ptr<Individual>;

	RandomSelectionAlg(const uint32_t repetitionsNum, std::function<IndividualPtr(void)> createRandomFun);
	IndividualPtr executeAlg();

private:
	const uint32_t repetitionsNum;
	std::function<IndividualPtr(void)> createRandomFun;
};

template<class Individual>
RandomSelectionAlg<Individual>::RandomSelectionAlg(const uint32_t repetitionsNum, std::function<IndividualPtr(void)> createRandomFun)
	: repetitionsNum(repetitionsNum)
	, createRandomFun(std::move(createRandomFun))
{
}

template<class Individual>
typename RandomSelectionAlg<Individual>::IndividualPtr RandomSelectionAlg<Individual>::executeAlg()
{
	IndividualPtr best;
	double bestFitness = -std::numeric_limits<double>::infinity();
	for (auto i = 0u; i < repetitionsNum; i++)
	{
		auto randomlySelected = createRandomFun();
		double fitness = randomlySelected->evaluate();
		if (fitness > bestFitness) {
			best = std::move(randomlySelected);
			bestFitness = fitness;
		}
	}
	return best;
}

} // namespace naive
