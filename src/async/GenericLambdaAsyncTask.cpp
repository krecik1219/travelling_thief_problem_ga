#include "GenericLambdaAsyncTask.hpp"

namespace async {

GenericLambdaAsyncTask::GenericLambdaAsyncTask(const std::function<std::vector<AsyncSubTaskPtr>(const uint32_t)>& splitFn,
	const uint32_t defaultSuggestedSubTasksNum)
	: splitFn(splitFn)
	, defaultSuggestedSubTasksNum(defaultSuggestedSubTasksNum)
{
}

GenericLambdaAsyncTask::~GenericLambdaAsyncTask() = default;

GenericLambdaAsyncTask::GenericLambdaAsyncTask(GenericLambdaAsyncTask&&) = default;

GenericLambdaAsyncTask & GenericLambdaAsyncTask::operator=(GenericLambdaAsyncTask&&) = default;

std::vector<AsyncSubTaskPtr> async::GenericLambdaAsyncTask::split()
{
	return splitFn(defaultSuggestedSubTasksNum);
}

std::vector<AsyncSubTaskPtr> GenericLambdaAsyncTask::split(const uint32_t suggestedSubTasksNum)
{
	return splitFn(suggestedSubTasksNum);
}

} // namespace async
