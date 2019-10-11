#include "StringUtils.hpp"

namespace utils {
namespace str {

std::string& ltrim(std::string& s, const char* whitespaces)
{
	s.erase(0, s.find_first_not_of(whitespaces));
	return s;
}

std::string& rtrim(std::string& s, const char* whitespaces)
{
	s.erase(s.find_last_not_of(whitespaces) + 1);
	return s;
}

 std::string& trim(std::string& s, const char* whitespaces)
{
	return ltrim(rtrim(s, whitespaces), whitespaces);
}

} // namespace str
} // namespace utils
