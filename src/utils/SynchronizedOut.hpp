#pragma once

#include <iostream>
#include <mutex>

namespace utils {

class SynchronizedOut final
{
public:
	~SynchronizedOut();
	SynchronizedOut(const SynchronizedOut&) = delete;
	SynchronizedOut& operator=(const SynchronizedOut&) = delete;
	SynchronizedOut(SynchronizedOut&&) = delete;
	SynchronizedOut& operator=(SynchronizedOut&&) = delete;

	static SynchronizedOut& getInstance();

	void lock();
	void release();

	template<class T>
	SynchronizedOut& operator<<(T&& t);

private:
	SynchronizedOut();

	std::mutex mutex;
};

template<class T>
SynchronizedOut& SynchronizedOut::operator<<(T&& t)
{
	std::cout << t;
	return *this;
}

} // namespace utils
