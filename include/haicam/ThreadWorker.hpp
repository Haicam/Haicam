#pragma once

#include "haicam/Context.hpp"
#include "haicam/ByteBuffer.hpp"
#include <functional>

namespace haicam
{
    class ThreadWorker
    {
    private:
        Context *context;
        uv_work_t req;

        static void work(uv_work_t *req)
        {
            ThreadWorker *thiz = (ThreadWorker *)req->data;
            thiz->run();
        }

        static void afterWork(uv_work_t *req, int status)
        {
            ThreadWorker *thiz = (ThreadWorker *)req->data;
            if (status == UV_ECANCELED)
                thiz->then();
            else
                thiz->cancelled();
        }

    public:
        ThreadWorker(Context *context) : context(context), runnable(NULL), afterRunCallback(NULL), afterCancelCallback(NULL)
        {
        }

        virtual ~ThreadWorker()
        {
        }

        void start()
        {
            req.data = (void *)this;
            uv_queue_work(context->uv_loop, &req, ThreadWorker::work, ThreadWorker::afterWork);
        }

        int cancel()
        {
            return uv_cancel((uv_req_t *)&req);
        }

        virtual ThreadWorker* run(std::function<void()> action = NULL)
        {
            if(action) {
                runnable = action;
                return this;
            }

            if(runnable) runnable();
            return NULL;
        }

        virtual ThreadWorker* then(std::function<void()> action = NULL)
        {
            if(action) {
                afterRunCallback = action;
                return this;
            }

            if(afterRunCallback) afterRunCallback();
            return NULL;
        }

        virtual ThreadWorker* cancelled(std::function<void()> action = NULL)
        {
            if(action) {
                afterCancelCallback = action;
                return this;
            }

            if(afterCancelCallback) afterCancelCallback();
            return NULL;
        }

        std::function<void()> runnable;
        std::function<void()> afterRunCallback;
        std::function<void()> afterCancelCallback;
    };

}