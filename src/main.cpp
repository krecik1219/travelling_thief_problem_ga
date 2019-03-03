#include <iostream>
#include <random>

#include <loader/InstanceLoader.hpp>
#include <loader/ConfigParsingException.hpp>
#include <ttp/TspInstance.hpp>

int main()
{
	std::random_device rd;
	std::mt19937 g(rd());
	std::cout << "before loader" << std::endl;
	loader::InstanceLoader loader;
	try
	{
		auto ttpConfigBase = loader.loadTtpConfig("data/easy_0.ttp");
		auto rndTsp = ttp::TspInstance::createRandom(ttpConfigBase.getConfig(), g);
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
