#pragma once

#include <string>
#include <configuration/TtpConfigBase.hpp>
#include <configuration/TtpConfig.hpp>


namespace loader {

enum class ReadingType
{
	city,
	item
};

class InstanceLoader
{
public:
	config::TtpConfigBase loadTtpConfig(const std::string& filePath) const;

private:
	void decideWhatToDoWithLine(const std::string& line, ReadingType& readingType, config::TtpConfig& ttpConfig) const;
	std::string prepareValueToStore(const std::string& s) const;
	void storeCityData(const std::string& line, config::TtpConfig& ttpConfig) const;
	void storeItemData(const std::string& line, config::TtpConfig& ttpConfig) const;
};
} // namespace loader

