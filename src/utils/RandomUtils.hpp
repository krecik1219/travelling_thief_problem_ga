#pragma once

#include <cstdint>
#include <random>
#include <mutex>

namespace utils {
namespace rnd {

// This class is thread-safe, but unefficiently - singleton methods are protected
class Random final
{
public:
	static Random& getInstance();

	Random(const Random&) = delete;
	Random(Random&&) = delete;
	~Random() = default;
	void operator=(const Random&) = delete;
	void operator=(Random&&) = delete;

	uint32_t getRandomUint(const uint32_t min, const uint32_t max);
	int32_t getRandomInt(const int32_t min, const int32_t max);
	double getRandomDouble(const double min, const double max);
	std::mt19937& getRndGen();

private:
	Random();

	std::random_device rd;
	std::mt19937 gen;
	std::uniform_int_distribution<uint32_t> uintDis;
	std::uniform_int_distribution<int32_t> intDis;
	std::uniform_real_distribution<double> doubleDis;

	std::mutex m;
};

} // namespace rnd
} // namespace utils
