#pragma once

#include <string>
#include <ttp/TtpConfigBase.hpp>


namespace loader {

enum class ReadingType
{
	city,
	item
};

class InstanceLoader
{
public:
	ttp::TtpConfigBase loadTtpConfig(const std::string& filePath) const;

private:
	void decideWhatToDoWithLine(const std::string& line, ReadingType& readingType, ttp::TtpConfig& ttpConfig) const;
	std::string prepareValueToStore(const std::string& s) const;
	void storeCityData(const std::string& line, ttp::TtpConfig& ttpConfig) const;
	void storeItemData(const std::string& line, ttp::TtpConfig& ttpConfig) const;
};
} // namespace loader

