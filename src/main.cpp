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
#include <naive/GreedyAlg.hpp>
#include <naive/RandomSelectionAlg.hpp>

using namespace std::chrono_literals;

int main(int argc, char **argv)
{
	std::string suffix;
	if (argc == 2)
		suffix = std::string(argv[1]);
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
		logging::Logger logger(gAlgConfig.resultsCsvFile + suffix);
		ga::GAlg<ttp::TtpIndividual> gAlg(gAlgConfig.gAlgParams, createRandomFun, logger);

		gAlg.run();

		logging::Logger logger2(gAlgConfig.bestIndividualResultFile + suffix);

		auto bestIndividual = gAlg.getBestIndividual();

		logger2.log("%s", bestIndividual->getStringRepresentation().c_str());


		logging::Logger logger3(gAlgConfig.bestGreedyAlgPath + suffix);
		naive::GreedyAlg<ttp::TtpIndividual> greedyAlg(gAlgConfig.naiveRepetitions, ttpConfig);
		auto bestFromGreedy = greedyAlg.executeAlg();
		logger3.log("%s", bestFromGreedy->getStringRepresentation().c_str());

		logging::Logger logger4(gAlgConfig.bestRandomAlgPath + suffix);
		naive::RandomSelectionAlg<ttp::TtpIndividual> rndAlg(gAlgConfig.naiveRepetitions, createRandomFun);
		auto bestFromRandom = rndAlg.executeAlg();
		logger4.log("%s", bestFromRandom->getStringRepresentation().c_str());
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
