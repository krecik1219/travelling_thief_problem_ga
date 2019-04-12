#include "Logger.hpp"

#include <cstdarg>
#include <cstdio>

namespace logging {

Logger::Logger(const std::string& logFilePath, const uint32_t bufferSize)
	: outputStream(logFilePath, std::ios::out)
	, bufferSize(bufferSize)
{
}

void Logger::logNewLine()
{
	outputStream << std::endl;
}

}  //namespace logging

