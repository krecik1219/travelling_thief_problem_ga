#include "SynchronizedOut.hpp"

namespace utils {


SynchronizedOut::SynchronizedOut() = default;

SynchronizedOut::~SynchronizedOut() = default;

SynchronizedOut& SynchronizedOut::getInstance()
{
	static SynchronizedOut instance;
	return instance;
}

void SynchronizedOut::lock()
{
	mutex.lock();
}

void SynchronizedOut::release()
{
	mutex.unlock();
}

} // namespace utils

