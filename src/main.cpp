#include <iostream>
#include <random>
#include <chrono>

#include <loader/InstanceLoader.hpp>
#include <loader/GAlgConfigLoader.hpp>
#include <loader/ConfigParsingException.hpp>
#include <ttp/TtpIndividual.hpp>
#include <ttp/Knapsack.hpp>
#include <ga/GAlg.hpp>
#include <logger/Logger.hpp>

using namespace std::chrono_literals;

int main()
{
	std::random_device rd;
	std::mt19937 g(rd());
	std::cout << "starting" << std::endl;
	try
	{
		loader::InstanceLoader instanceLoader;
		loader::GAlgConfigLoader gAlgConfigLoader;
		auto gAlgConfigBase = gAlgConfigLoader.loadGAlgConfig("gaConfig.txt");
		const auto& gAlgConfig = gAlgConfigBase.getConfig();
		auto ttpConfigBase = instanceLoader.loadTtpConfig(gAlgConfig.instanceFilePath);
		auto ttpConfig = ttpConfigBase.getConfig();
		auto createRandomFun = [&ttpConfig, &g]() {return ttp::TtpIndividual::createRandom(ttpConfig, g); };
		logging::Logger logger(gAlgConfig.resultsCsvFile);
		ga::GAlg<ttp::TtpIndividual> gAlg(gAlgConfig.gAlgParams, createRandomFun, logger);

		gAlg.run();

		logging::Logger logger2(gAlgConfig.bestIndividualResultFile);

		auto bestIndividual = gAlg.getBestIndividual();

		logger2.log("%s", bestIndividual->getStringRepresentation().c_str());

		//auto rndTtp1 = ttp::TtpIndividual::createRandom(ttpConfigBase.getConfig(), g);
		//auto rndTtp2 = ttp::TtpIndividual::createRandom(ttpConfigBase.getConfig(), g);
		//auto fitness1 = rndTtp1->evaluate();
		//auto fitness2 = rndTtp2->evaluate();
		//auto offspring = rndTtp1->crossover(*rndTtp2);
		//auto fitnessOfOffspring = offspring->evaluate();
		//offspring->mutation();
		//auto fitnessAfterMut = offspring->evaluate();
		//std::cout << "fitness1: " << fitness1 << std::endl;
		//std::cout << "fitness2: " << fitness2 << std::endl;
		//std::cout << "fitnessOfOffspring: " << fitnessOfOffspring << std::endl;
		//std::cout << "fitnessAfterMut: " << fitnessAfterMut << std::endl;
		//std::cout << "random created" << std::endl;
	}
	catch (loader::ConfigParsingException& e)
	{
		std::cout << "Parsing error: " + std::string(e.what()) << std::endl;
	}
	catch (std::exception& e)
	{
		std::cout << "unknown error: " + std::string(e.what()) << std::endl;
	}

	std::cout << "finished" << std::endl;
	//getchar(); getchar();
	return 0;
}
