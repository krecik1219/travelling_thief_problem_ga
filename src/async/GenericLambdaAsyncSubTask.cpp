#include "GenericLambdaAsyncSubTask.hpp"

namespace async {

GenericLambdaAsyncSubTask::GenericLambdaAsyncSubTask(const std::function<void()>& runFn)
	: runFn(runFn)
{
}

GenericLambdaAsyncSubTask::~GenericLambdaAsyncSubTask() = default;

GenericLambdaAsyncSubTask::GenericLambdaAsyncSubTask(GenericLambdaAsyncSubTask &&) = default;

GenericLambdaAsyncSubTask& GenericLambdaAsyncSubTask::operator=(GenericLambdaAsyncSubTask &&) = default;

void GenericLambdaAsyncSubTask::run()
{
	runFn();
}

} // namespace async
