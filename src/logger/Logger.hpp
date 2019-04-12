#pragma once

#include <string>
#include <fstream>


namespace logging {

class Logger
{
public:
	Logger(const std::string& logFilePath, uint32_t bufferSize = 10000u);

	template<class... Args>
	void log(const char* format, Args... args);
	void logNewLine();

private:
	std::ofstream outputStream;
	uint32_t bufferSize;
};

template<class... Args>
void Logger::log(const char* format, Args... args)
{
	std::unique_ptr<char[]> buffer = std::make_unique<char[]>(bufferSize);
	std::snprintf(buffer.get(), bufferSize, format, args...);
	outputStream << buffer << std::endl;
}

} // namespace logging
