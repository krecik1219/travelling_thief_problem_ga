#include "ExecutionManager.hpp"

#include <algorithm>
#include <thread>

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
					std::lock_guard<std::mutex> lock(mutexOnAvailableExecutors);
					availableExecutorsIds.push(executorId);
				}
				conditionOnAvailableExecutors.notify_all();
			},
			i
		));
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
		std::vector<AsyncSubTaskPtr> subTasks = task->split();
		while (!subTasks.empty())
		{
			std::unique_lock<std::mutex> lockOnAvialableExecutors(mutexOnAvailableExecutors);
			conditionOnAvailableExecutors.wait(lockOnAvialableExecutors, [this]() {
				return !availableExecutorsIds.empty();
			});
			sendToExecutor(std::move(subTasks.back()));
			subTasks.pop_back();
		}
	});
	executionDispatcherThread.detach();
}

void ExecutionManager::waitForExecutionDone()
{
	for (auto& executor : executors)
		executor->join();
}

void ExecutionManager::sendToExecutor(AsyncSubTaskPtr&& subTask)
{
	if (!availableExecutorsIds.empty())
	{
		const uint32_t executorId = availableExecutorsIds.front();
		availableExecutorsIds.pop();
		executors[executorId]->run(std::move(subTask));
	}
}

} // namespace async

