#include "TtpIndividual.hpp"

namespace ttp {

TtpIndividual::TtpIndividual(const TtpConfig& ttpConfig, TspSolution&& tsp, KnapsackPtr&& knapsack)
	: ttpConfig(ttpConfig)
	, tsp(std::move(tsp))
	, knapsack(std::move(knapsack))
{
}

float TtpIndividual::computeFitness() const
{
	return knapsack->getKnapsackValue() - tsp.getTripTime(*knapsack, ttpConfig);
}

void TtpIndividual::mutation()
{

}

} // namespace ttp
