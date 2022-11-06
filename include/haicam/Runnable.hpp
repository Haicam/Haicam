#ifndef __HAICAM_RUNNABLE_HPP__
#define __HAICAM_RUNNABLE_HPP__

#include "haicam/Context.hpp"
#include "haicam/SafeQueue.hpp"
#include "haicam/ByteBuffer.hpp"
#include <functional>
#include "haicam/Timer.hpp"

namespace haicam
{

    class Runnable
    {
    private:
        static void asyncCloseCallback(uv_handle_t *handle)
        {
        }

        static void asyncCallback(uv_async_t *handle)
        {
            Runnable *thiz = static_cast<Runnable *>(handle->data);
            ByteBufferPtr buf;
            while (thiz->output.dequeue(buf))
            {
                if (thiz->onSuccessCallback != NULL)
                {
                    thiz->onSuccessCallback(buf);
                }
            }
        }

        static void timerCallback(Runnable* thiz)
        {
            thiz->stop();
            if (thiz->onTimeoutCallback != NULL)
            {
                thiz->onTimeoutCallback();
            }
        }

        static void process(void *arg)
        {
            Runnable *thiz = static_cast<Runnable *>(arg);
            thiz->run();
        }

    protected:
        virtual void run() = 0;

        bool isRunning()
        {
            return input.isOpen();
        }

        void sendDataOut(ByteBufferPtr buf)
        {
            if (timer)
            {
                timer->stop();
            }

            uv_async_send(&this->async);
            this->output.enqueue(buf);
        }

    public:
        Runnable(Context *context)
            : context(context), input(), output(), onSuccessCallback(NULL), onTimeoutCallback(NULL), async(), timer(), thread(), isStopped(true), timeoutMillisecs(0){};
        ~Runnable(){};

        void start(int timeoutMillisecs = 0)
        {
            if (!isStopped)
                return;
            isStopped = false;

            H_ASSERT(context->uv_loop != NULL);
            H_ASSERT(isRunning());

            if (timeoutMillisecs > 0)
            {
                timer = Timer::create(context, timeoutMillisecs, 0);
                timer->onTimeoutCallback = std::bind(Runnable::timerCallback, this);
                timer->start();
            }

            uv_async_init(context->uv_loop, &async, Runnable::asyncCallback);
            async.data = static_cast<void *>(this);
            uv_thread_create(&this->thread, Runnable::process, this);
        }

        void sendDataIn(ByteBufferPtr data)
        {
            input.enqueue(data);
        }

        void stop()
        {
            if (isStopped)
                return;
            isStopped = true;

            input.close();
            output.close();

            if (timer)
            {
                timer->stop();
            }

            uv_close((uv_handle_t *)&async, Runnable::asyncCloseCallback);
            uv_thread_join(&this->thread);
        }

    private:
        Context *context;
        SafeQueue<ByteBufferPtr> output;
        uv_async_t async;
        TimerPtr timer;
        uv_thread_t thread;
        int timeoutMillisecs;

        //used in main thread only
        bool isStopped;

    protected:
        SafeQueue<ByteBufferPtr> input;

    public:
        std::function<void(ByteBufferPtr)> onSuccessCallback;
        std::function<void()> onTimeoutCallback;
    };
}

#endif