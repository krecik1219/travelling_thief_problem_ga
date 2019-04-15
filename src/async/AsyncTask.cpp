#include "AsyncTask.hpp"

namespace async {

AsyncTask::AsyncTask() = default;

AsyncTask::~AsyncTask() = default;

AsyncTask::AsyncTask(AsyncTask&&) = default;

AsyncTask & AsyncTask::operator=(AsyncTask&&) = default;

} // namespace async

