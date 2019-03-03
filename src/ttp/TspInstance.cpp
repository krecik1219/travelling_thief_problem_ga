#include "TspInstance.hpp"

#include <algorithm>

namespace ttp {

TspInstance::TspInstance(std::vector<City>&& cities)
	: cityChain(std::move(cities))
{
}

} // namespace ttp
