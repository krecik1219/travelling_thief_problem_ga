#pragma once

#include <cstdint>
#include <functional>

#include "AsyncTask.hpp"

namespace async {

class GenericLambdaAsyncTask : public AsyncTask
{
public:
	GenericLambdaAsyncTask(
		const std::function<std::vector<AsyncSubTaskPtr>(const uint32_t)>& splitFn,
		const uint32_t defaultSuggestedSubTasksNum);

	virtual ~GenericLambdaAsyncTask();

	GenericLambdaAsyncTask(const GenericLambdaAsyncTask&) = delete;
	GenericLambdaAsyncTask& operator=(const GenericLambdaAsyncTask&) = delete;
	GenericLambdaAsyncTask(GenericLambdaAsyncTask&&);
	GenericLambdaAsyncTask& operator=(GenericLambdaAsyncTask&&);

	virtual std::vector<AsyncSubTaskPtr> split() override;
	virtual std::vector<AsyncSubTaskPtr> split(const uint32_t suggestedSubTasksNum) override;

private:
	std::function<std::vector<AsyncSubTaskPtr>(const uint32_t)> splitFn;
	uint32_t defaultSuggestedSubTasksNum;
};

} // namespace async
