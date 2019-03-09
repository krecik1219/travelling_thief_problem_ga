#include "Logger.hpp"

#include <cstdarg>
#include <cstdio>

namespace logging {

Logger::Logger(const std::string& logFilePath)
	: outputStream(logFilePath, std::ios::out)
{
}

}  //namespace logging

