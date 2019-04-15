#pragma once

#include <thread>
#include <atomic>
#include <memory>

#include "AsyncSubTask.hpp"

namespace async {

using AsyncSubTaskPtr = std::unique_ptr<AsyncSubTask>;

class Executor
{
public:
	Executor(const std::function<void(const uint32_t)>& notifyAvailabilityFn, const uint32_t executorId);
	~Executor();

	Executor(const Executor&) = delete;
	Executor& operator=(const Executor&) = delete;
	Executor(Executor&&) = delete;
	Executor& operator=(Executor&&) = delete;

	void run(AsyncSubTaskPtr&& subTask);
	void join();

	bool isAvailable() const;

private:
	const uint32_t executorId;

	std::thread executionThread;

	std::atomic<bool> available;
	const std::function<void(const uint32_t)> notifyAvailabilityFn;
};

} // namespace async
