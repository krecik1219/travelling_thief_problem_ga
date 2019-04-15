#pragma once

namespace async {

class AsyncSubTask
{
public:
	AsyncSubTask();
	virtual ~AsyncSubTask();

	AsyncSubTask(const AsyncSubTask&) = delete;
	AsyncSubTask& operator=(const AsyncSubTask&) = delete;
	AsyncSubTask(AsyncSubTask&&);
	AsyncSubTask& operator=(AsyncSubTask&&);

	virtual void run() = 0;
};

} // namespace async
