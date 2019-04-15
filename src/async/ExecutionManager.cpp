#include "ExecutionManager.hpp"

#include <algorithm>
#include <thread>
#include <iostream>

namespace async {

ExecutionManager::ExecutionManager(AsyncTaskPtr&& task, const uint32_t numberOfExecutorsToUse)
	: numberOfExecutors(numberOfExecutorsToUse)
	, task(std::move(task))
{
	for (auto i = 0u; i < numberOfExecutors; i++)
	{
		executors.push_back(
			std::make_unique<Executor>([this](const uint32_t executorId)
			{
				{
					std::cout << "notify about availability trying to acquire lock. Thread id=" << std::this_thread::get_id << " executor id= " << executorId << std::endl;
					std::lock_guard<std::mutex> lock(mutexOnAvailableExecutors);
					std::cout << "notify about availability LOCK AQUIRED. Thread id=" << std::this_thread::get_id << " executor id= " << executorId << std::endl;
					availableExecutorsIds.push(executorId);
				}
				conditionOnAvailableExecutors.notify_all();
				std::cout << "notify about availability LOCK released, threads notified. Thread id=" << std::this_thread::get_id << std::endl;
			},
			i
		));
		availableExecutorsIds.push(i);
	}
}

ExecutionManager::ExecutionManager(AsyncTaskPtr&& task)
	: ExecutionManager(std::move(task), std::max(std::thread::hardware_concurrency(), minNumberOfExecutors))
{
}

ExecutionManager::~ExecutionManager()
{
	waitForExecutionDone();
}

void ExecutionManager::executeTask()
{
	executionDispatcherThread = std::thread([this]() {
		std::vector<AsyncSubTaskPtr> subTasks = task->split(numberOfExecutors);
		while (!subTasks.empty())
		{
			std::cout << "execution dispatcher jus befory lock acquire try. Thread id=" << std::this_thread::get_id << std::endl;
			std::unique_lock<std::mutex> lockOnAvialableExecutors(mutexOnAvailableExecutors);
			conditionOnAvailableExecutors.wait(lockOnAvialableExecutors, [this]() {
				return !availableExecutorsIds.empty();
			});
			std::cout << "condition variable passed, just before sending to executor. Thread id=" << std::this_thread::get_id << std::endl;
			sendToExecutor(std::move(subTasks.back()));
			subTasks.pop_back();
		}
	});
}

void ExecutionManager::waitForExecutionDone()
{
	if (executionDispatcherThread.joinable())
		executionDispatcherThread.join();
	for (auto& executor : executors)
		executor->join();
}

void ExecutionManager::sendToExecutor(AsyncSubTaskPtr&& subTask)
{
	if (!availableExecutorsIds.empty())
	{
		const uint32_t executorId = availableExecutorsIds.front();
		std::cout << "Thread id: " << std::this_thread::get_id << "sending to executor with id: " << executorId << std::endl;
		availableExecutorsIds.pop();
		executors[executorId]->run(std::move(subTask));
	}
	std::cout << "after sending to executor. Thread id=" << std::this_thread::get_id << std::endl;
}

} // namespace async

