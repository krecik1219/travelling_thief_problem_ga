#include "InstanceLoader.hpp"

#include <array>
#include <exception>
#include <fstream>
#include <sstream>

#include <ttp/TtpConfig.hpp>
#include <utils/StringUtils.hpp>
#include "ConfigParsingException.hpp"

namespace loader
{

ttp::TtpConfigBase InstanceLoader::loadTtpConfig(const std::string& filePath) const
{
	std::ifstream fileHandle(filePath);
	if (!fileHandle.is_open())
		throw std::runtime_error("Could not read file: " + filePath);

	ttp::TtpConfig ttpConfig;
	std::string line;
	ReadingType readingType = ReadingType::city;
	while (std::getline(fileHandle, line))
	{
		decideWhatToDoWithLine(line, readingType, ttpConfig);
	}

	ttpConfig.fillNearestDistanceLookup();
	return ttp::TtpConfigBase(std::move(ttpConfig));
}

void InstanceLoader::decideWhatToDoWithLine(const std::string& line, ReadingType& readingType, ttp::TtpConfig& ttpConfig) const
{
	if (line.find("PROBLEM NAME:") != std::string::npos)
	{
		auto value = prepareValueToStore(line);
		ttpConfig.problemName = value;
	}
	else if (line.find("KNAPSACK DATA TYPE:") != std::string::npos)
	{
		auto value = prepareValueToStore(line);
		ttpConfig.knapsackDataType = value;
	}
	else if (line.find("DIMENSION:") != std::string::npos)
	{
		auto value = prepareValueToStore(line);
		ttpConfig.dimenssion = std::stoi(value);
	}
	else if (line.find("NUMBER OF ITEMS:") != std::string::npos)
	{
		auto value = prepareValueToStore(line);
		ttpConfig.itemsNum = std::stoi(value);
	}
	else if (line.find("CAPACITY OF KNAPSACK:") != std::string::npos)
	{
		auto value = prepareValueToStore(line);
		ttpConfig.capacityOfKnapsack = std::stoi(value);
	}
	else if (line.find("MIN SPEED:") != std::string::npos)
	{
		auto value = prepareValueToStore(line);
		ttpConfig.minVelocity = std::stof(value);
	}
	else if (line.find("MAX SPEED:") != std::string::npos)
	{
		auto value = prepareValueToStore(line);
		ttpConfig.maxVelocity = std::stof(value);
	}
	else if (line.find("RENTING RATIO:") != std::string::npos)
	{
		auto value = prepareValueToStore(line);
		ttpConfig.rentingRatio = std::stof(value);
	}
	else if (line.find("NODE_COORD_SECTION") != std::string::npos)
	{
		readingType = ReadingType::city;
	}
	else if (line.find("ITEMS SECTION") != std::string::npos)
	{
		readingType = ReadingType::item;
	}
	else if (line.find("EDGE_WEIGHT_TYPE:") != std::string::npos)
	{
		// nothing, just skip it
	}
	else
	{
		if (readingType == ReadingType::city)
			storeCityData(line, ttpConfig);
		else
			storeItemData(line, ttpConfig);
	}
}

std::string InstanceLoader::prepareValueToStore(const std::string& s) const
{
	auto pos = s.find(':');
	if (pos == std::string::npos)
		throw ConfigParsingException("Bad file structure at line: \"" + s + "\"");
	auto value = s.substr(pos + 1, s.length() - pos - 1);
	utils::str::trim(value);
	return value;
}

void InstanceLoader::storeCityData(const std::string & line, ttp::TtpConfig& ttpConfig) const
{
	std::istringstream ss(line);
	std::string token;
	std::array<std::string, 3> tokens;;
	uint32_t counter = 0;
	try
	{
		while (std::getline(ss, token, '\t'))
		{
			tokens.at(counter) = token;
			counter++;
		}
	}
	catch (std::out_of_range&)
	{
		throw ConfigParsingException("Parsing city data - bad file structure: " + line);
	}
	auto index = static_cast<uint32_t>(std::stoi(tokens[0]));
	ttpConfig.cities.emplace_back(ttp::City{ index, std::stof(tokens[1]), std::stof(tokens[2]) });
}

void InstanceLoader::storeItemData(const std::string & line, ttp::TtpConfig& ttpConfig) const
{
	std::istringstream ss(line);
	std::string token;
	std::array<std::string, 4> tokens;;
	uint32_t counter = 0;
	try
	{
		while (std::getline(ss, token, '\t'))
		{
			tokens.at(counter) = token;
			counter++;
		}
	}
	catch (std::out_of_range&)
	{
		throw ConfigParsingException("Parsing item data - bad file structure: " + line);
	}
	auto index = static_cast<uint32_t>(std::stoi(tokens[0]));
	auto profit = static_cast<uint32_t>(std::stoi(tokens[1]));
	auto weight = static_cast<uint32_t>(std::stoi(tokens[2]));
	auto cityId = static_cast<uint32_t>(std::stoi(tokens[3]));
	ttpConfig.items.emplace_back(ttp::Item{ index, profit, weight, cityId });
}

} // namespace loader
