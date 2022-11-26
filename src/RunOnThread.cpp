#include "haicam/RunOnThread.hpp"

using namespace haicam;

void RunOnThread::asyncCloseCallback(uv_handle_t *handle)
{
}

void RunOnThread::asyncCallback(uv_async_t *handle)
{
    RunOnThread *thiz = static_cast<RunOnThread *>(handle->data);

    std::function<void()> callback;
    while (thiz->callbackQueue.dequeue(callback))
    {
        callback();
    }
}

RunOnThread::RunOnThread(Context *context)
    : context(context), callbackQueue(), async(), isStopped(true){

                                                  };

RunOnThread::~RunOnThread()
{
    stopSchedule();
};

void RunOnThread::startSchedule()
{
    if (!isStopped)
        return;
    isStopped = false;

    H_ASSERT(context->uv_loop != NULL);

    uv_async_init(context->uv_loop, &async, RunOnThread::asyncCallback);
    async.data = static_cast<void *>(this);
}

void RunOnThread::schedule(std::function<void()> callback)
{
    this->callbackQueue.enqueue(callback);
    uv_async_send(&this->async);
}

void RunOnThread::stopSchedule()
{
    if (isStopped)
        return;
    isStopped = true;

    callbackQueue.close();

    uv_close((uv_handle_t *)&async, RunOnThread::asyncCloseCallback);
}
