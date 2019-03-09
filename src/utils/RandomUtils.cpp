#include "RandomUtils.hpp"

namespace utils {
namespace rnd {

Random& Random::getInstance()
{
	static Random instance;
	return instance;
}

uint32_t Random::getRandomUint(const uint32_t min, const uint32_t max)
{
	std::uniform_int_distribution<uint32_t>::param_type distParam(min, max);
	uintDis.param(distParam);
	return uintDis(gen);
}

int32_t Random::getRandomInt(const int32_t min, const int32_t max)
{
	std::uniform_int_distribution<int32_t>::param_type distParam(min, max);
	intDis.param(distParam);
	return intDis(gen);
}

double Random::getRandomDouble(const double min, const double max)
{
	std::uniform_real_distribution<double>::param_type distParam(min, max);
	doubleDis.param(distParam);
	return doubleDis(gen);
}

std::mt19937 & Random::getRndGen()
{
	return gen;
}

Random::Random()
	: rd()
	, gen(rd())
{
}

} // namespace rnd
} // namespace utils