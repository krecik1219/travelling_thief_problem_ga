#include "Executor.hpp"

#include <iostream>

namespace async {

Executor::Executor(const std::function<void(const uint32_t)>& notifyAvailabilityFn, const uint32_t executorId)
	: executorId(executorId)
	, available(true)
	, notifyAvailabilityFn(notifyAvailabilityFn)
{
}

Executor::~Executor()
{
	if(executionThread.joinable())
		executionThread.join();
}

void Executor::run(AsyncSubTaskPtr&& subTask)
{
	std::cout << "executor run, just before atomic store. Thread id=" << std::this_thread::get_id << std::endl;
	available.store(false);
	if (executionThread.joinable())
	{
		std::cout << "executionThread join waiting. Thread id=" << std::this_thread::get_id << std::endl;
		executionThread.join();
	}
	std::cout << "run before thread creation. Thread id=" << std::this_thread::get_id << std::endl;
	executionThread = std::thread([this](AsyncSubTaskPtr&& subTask) {
			std::cout << "running subtask. Thread id=" << std::this_thread::get_id << std::endl;
			subTask->run();
			available.store(true);
			notifyAvailabilityFn(executorId);
		},
		std::move(subTask)
	);
}

void Executor::join()
{
	if (executionThread.joinable())
		executionThread.join();
}

bool Executor::isAvailable() const
{
	return available.load();
}

} // namespace async
