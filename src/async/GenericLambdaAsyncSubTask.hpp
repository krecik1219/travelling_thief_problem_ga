#pragma once

#include <functional>

#include "AsyncSubTask.hpp"

namespace async {

class GenericLambdaAsyncSubTask : public AsyncSubTask
{
public:
	GenericLambdaAsyncSubTask(const std::function<void()>& runFn);

	virtual ~GenericLambdaAsyncSubTask();

	GenericLambdaAsyncSubTask(const GenericLambdaAsyncSubTask&) = delete;
	GenericLambdaAsyncSubTask& operator=(const GenericLambdaAsyncSubTask&) = delete;
	GenericLambdaAsyncSubTask(GenericLambdaAsyncSubTask&&);
	GenericLambdaAsyncSubTask& operator=(GenericLambdaAsyncSubTask&&);

	virtual void run() override;

private:
	std::function<void()> runFn;
};

} // namespace async
