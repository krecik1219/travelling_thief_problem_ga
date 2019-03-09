#pragma once

#include <string>
#include <configuration/GAlgConfigBase.hpp>
#include <configuration/GAlgConfig.hpp>


namespace loader {

class GAlgConfigLoader
{
public:
	config::GAlgConfigBase loadGAlgConfig(const std::string& filePath) const;

private:
	void decideWhatToDoWithLine(const std::string& line, config::GAlgConfig& ttpConfig) const;
	std::string prepareValueToStore(const std::string& s) const;
};
} // namespace loader

