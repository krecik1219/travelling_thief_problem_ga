#pragma once

#include <condition_variable>
#include <cstdint>
#include <memory>
#include <queue>
#include <mutex>

#include "Executor.hpp"
#include "AsyncTask.hpp"
#include "AsyncSubTask.hpp"

namespace async {

using ExecutorPtr = std::unique_ptr<Executor>;
using AsyncTaskPtr = std::unique_ptr<AsyncTask>;

class ExecutionManager
{
public:
	ExecutionManager(AsyncTaskPtr&& task, const uint32_t numberOfExecutorsToUse);
	ExecutionManager(AsyncTaskPtr&& task);
	~ExecutionManager();

	ExecutionManager() = delete;
	ExecutionManager(const ExecutionManager&) = delete;
	ExecutionManager& operator=(const ExecutionManager&) = delete;
	ExecutionManager(ExecutionManager&&) = delete;
	ExecutionManager& operator=(ExecutionManager&&) = delete;

	void executeTask();
	void waitForExecutionDone();

private:
	static constexpr uint32_t minNumberOfExecutors = 16u;

	void sendToExecutor(AsyncSubTaskPtr&& subTask);

	std::queue<uint32_t> availableExecutorsIds;
	std::mutex mutexOnAvailableExecutors;
	std::condition_variable conditionOnAvailableExecutors;

	uint32_t numberOfExecutors;
	std::vector<ExecutorPtr> executors;
	AsyncTaskPtr task;
	std::thread executionDispatcherThread;
};

} // namespace async
