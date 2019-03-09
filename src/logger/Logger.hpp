#pragma once

#include <string>
#include <fstream>


namespace logging {

class Logger
{
public:
	Logger(const std::string& logFilePath);

	template<class... Args>
	void log(const char* format, Args... args);

private:
	std::ofstream outputStream;
};

template<class... Args>
void Logger::log(const char* format, Args... args)
{
	char buffer[10000];
	std::snprintf(buffer, sizeof(buffer), format, args...);
	outputStream << buffer << std::endl;
}

} // namespace logging
