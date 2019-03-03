#include <iostream>
#include <loader/InstanceLoader.hpp>
#include <loader/ConfigParsingException.hpp>

int main()
{
	std::cout << "before loader" << std::endl;
	loader::InstanceLoader loader;
	try
	{
		auto ttpConfigBase = loader.loadTtpConfig("data/easy_0.ttp");
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
