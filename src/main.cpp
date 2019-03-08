#include <iostream>
#include <random>

#include <loader/InstanceLoader.hpp>
#include <loader/ConfigParsingException.hpp>
#include <ttp/TtpIndividual.hpp>
#include <ttp/Knapsack.hpp>

int main()
{
	std::random_device rd;
	std::mt19937 g(rd());
	std::cout << "before loader" << std::endl;
	loader::InstanceLoader loader;
	try
	{
		auto ttpConfigBase = loader.loadTtpConfig("data/trivial_0.ttp");
		ttp::KnapsackPtr knapsack = std::make_shared<ttp::Knapsack>(ttpConfigBase.getConfig().capacityOfKnapsack);
		knapsack->fillKnapsack(ttpConfigBase.getConfig());
		auto rndTtp1 = ttp::TtpIndividual::createRandom(ttpConfigBase.getConfig(), knapsack, g);
		auto rndTtp2 = ttp::TtpIndividual::createRandom(ttpConfigBase.getConfig(), knapsack, g);
		auto fitness1 = rndTtp1->evaluate();
		auto fitness2 = rndTtp2->evaluate();
		auto offspring = rndTtp1->crossover(*rndTtp2);
		auto fitnessOfOffspring = offspring.evaluate();
		offspring.mutation();
		auto fitnessAfterMut = offspring.evaluate();
		std::cout << "fitness1: " << fitness1 << std::endl;
		std::cout << "fitness2: " << fitness2 << std::endl;
		std::cout << "fitnessOfOffspring: " << fitnessOfOffspring << std::endl;
		std::cout << "fitnessAfterMut: " << fitnessAfterMut << std::endl;
		std::cout << "random created" << std::endl;
	}
	catch (loader::ConfigParsingException& e)
	{
		std::cout << "Parsing error: " + std::string(e.what()) << std::endl;
	}
	catch (std::exception& e)
	{
		std::cout << "unknown error: " + std::string(e.what()) << std::endl;
	}

	std::cout << "After loader" << std::endl;
	getchar(); getchar();
	return 0;
}
