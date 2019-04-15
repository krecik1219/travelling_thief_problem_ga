#pragma once

#include <memory>
#include <vector>

#include "AsyncSubTask.hpp"

namespace async {

using AsyncSubTaskPtr = std::unique_ptr<AsyncSubTask>;

class AsyncTask
{
public:
	AsyncTask();
	virtual ~AsyncTask();

	AsyncTask(const AsyncTask&) = delete;
	AsyncTask& operator=(const AsyncTask&) = delete;
	AsyncTask(AsyncTask&&);
	AsyncTask& operator=(AsyncTask&&);

	virtual std::vector<AsyncSubTaskPtr> split() = 0;
};

}
