#include "haicam/RunOnMainThread.hpp"

using namespace haicam;

RunOnMainThread *RunOnMainThread::instance = NULL;

RunOnMainThread::RunOnMainThread(Context *context) : RunOnThread(context)
{
}

void RunOnMainThread::initOnMainThread(Context *context)
{
    if (instance == NULL)
    {
        instance = new RunOnMainThread(context);
        instance->startSchedule();
    }
}

RunOnMainThread *RunOnMainThread::getInstance()
{
    H_ASSERT(instance != NULL);

    return instance;
}
