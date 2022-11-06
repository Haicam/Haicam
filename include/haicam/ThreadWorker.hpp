#pragma once

#include "haicam/Context.hpp"
#include "haicam/ByteBuffer.hpp"
#include <functional>
#include <exception>
#include "haicam/Timer.hpp"

#ifdef HAICAM_PLATFORM_GM8136
#include <cstdatomic>
#else
#include <atomic>
#endif

namespace haicam
{
    template <class T>
    class ThreadWorker
    {
    private:
        Context *context;
        TimerPtr timer;
        uv_work_t req;
        T result;
        int errorCode;

        static void work(uv_work_t *req)
        {
            ThreadWorker *thiz = (ThreadWorker *)req->data;
            thiz->run();
        }

        static void afterWork(uv_work_t *req, int status)
        {
            ThreadWorker *thiz = (ThreadWorker *)req->data;
            if (status == UV_ECANCELED)
                thiz->cancelled();
            else
                thiz->then();
        }

        static void workTimeout(ThreadWorker * thiz)
        {
            thiz->timeout();
        }

    public:
        ThreadWorker(Context *context) : context(context), timer(), runnable(NULL), thenCallback(NULL), cancelCallback(NULL), timeoutCallback(NULL), errorCallback(NULL), result(), errorCode(0), running(true)
        {
        }

        virtual ~ThreadWorker()
        {
            if (timer)
            {
                timer->stop();
            }
        }

        void start(int timeoutMillisecs = 0)
        {
            if (timeoutMillisecs > 0)
            {
                timer = Timer::create(context, timeoutMillisecs, 0);
                timer->onTimeoutCallback = std::bind(ThreadWorker::workTimeout, this);
                timer->start();
            }

            req.data = (void *)this;
            uv_queue_work(context->uv_loop, &req, ThreadWorker::work, ThreadWorker::afterWork);
        }

        int cancel()
        {
            return uv_cancel((uv_req_t *)&req);
        }

        virtual ThreadWorker* run(std::function<T()> action = NULL)
        {
            if(action) {
                runnable = action;
                return this;
            }

            if(runnable) {
                try{
                    result = runnable();
                } catch (std::exception& e) {
                    errorCode = 1;
                }
            }
            return NULL;
        }

        virtual ThreadWorker* then(std::function<void(T)> action = NULL)
        {
            if(action) {
                thenCallback = action;
                return this;
            }

            if (timer)
            {
                timer->stop();
            }

            if(errorCode) {
                if(errorCallback) error();
                return NULL;
            }

            if(thenCallback) thenCallback(result);
            return NULL;
        }

        virtual ThreadWorker* cancelled(std::function<void()> action = NULL)
        {
            if(action) {
                cancelCallback = action;
                return this;
            }

            if (timer)
            {
                timer->stop();
            }

            if(cancelCallback) cancelCallback();
            return NULL;
        }
        
        virtual ThreadWorker* timeout(std::function<void()> action = NULL)
        {
            if(action) {
                timeoutCallback = action;
                return this;
            }

            running = false;

            if(timeoutCallback) timeoutCallback();
            return NULL;
        }

        virtual ThreadWorker* error(std::function<void(int)> action = NULL)
        {
            if(action) {
                errorCallback = action;
                return this;
            }

            if(errorCallback) errorCallback(errorCode);
            return NULL;
        }

        std::function<T()> runnable;
        std::function<void(T)> thenCallback;
        std::function<void()> cancelCallback;
        std::function<void()> timeoutCallback;
        std::function<void(int)> errorCallback;
        
        std::atomic<bool> running;
    };

}