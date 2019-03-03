#pragma once

#include <string>


namespace utils {
namespace str {

std::string& ltrim(std::string& s, const char* whitespaces = " \t\n\r\f\v");
std::string& rtrim(std::string& s, const char* whitespaces = " \t\n\r\f\v");
std::string& trim(std::string& s, const char* whitespaces = " \t\n\r\f\v");

}  // namespace str
} // namespace utils
