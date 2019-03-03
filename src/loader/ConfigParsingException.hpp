#pragma once

#include <stdexcept>

namespace loader {

class ConfigParsingException : public std::runtime_error
{
public:
	ConfigParsingException() = default;
	ConfigParsingException(const std::string& msg)
		: std::runtime_error(msg)
	{
	}
};

} // namespace loader
