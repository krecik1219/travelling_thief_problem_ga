#include "Executor.hpp"

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
	available.store(false);
	if (executionThread.joinable())
	{
		executionThread.join();
	}
	executionThread = std::thread([this](AsyncSubTaskPtr&& subTask) {
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
