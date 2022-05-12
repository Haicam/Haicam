#ifndef __HAICAM_RUNNABLE_HPP__
#define __HAICAM_RUNNABLE_HPP__

#include "haicam/Context.hpp"
#include "haicam/SafeQueue.hpp"
#include "haicam/ByteBuffer.hpp"
#include <functional>

namespace haicam
{

    class Runnable
    {
    private:
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

        static void timerCallback(uv_timer_t *handle)
        {
            uv_timer_stop(handle);
            Runnable *thiz = static_cast<Runnable *>(handle->data);
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

        void sendDataOut(ByteBufferPtr buf)
        {
            uv_timer_stop(&timer);

            uv_async_send(&this->async);
            this->output.enqueue(buf);
        }

    public:
        Runnable(Context *context)
            : context(context), input(), output(), onSuccessCallback(NULL), onTimeoutCallback(NULL), async(), timer(), thread(){};
        ~Runnable(){};

        void start(int timeoutMillisecs = 60000)
        {
            uv_timer_init(context->uv_loop, &timer);
            timer.data = static_cast<void *>(this);
            uv_timer_start(&timer, Runnable::timerCallback, 0, timeoutMillisecs);

            this->async.data = static_cast<void *>(this);
            uv_async_init(context->uv_loop, &this->async, Runnable::asyncCallback);
            uv_thread_create(&this->thread, Runnable::process, this);
        }

        void sendDataIn(ByteBufferPtr data)
        {
            input.enqueue(data);
        }

        void stop()
        {
            input.close();
            output.close();
            uv_close((uv_handle_t *)&this->async, NULL);
            uv_thread_join(&this->thread);
        }

    private:
        Context *context;
        SafeQueue<ByteBufferPtr> output;
        uv_async_t async;
        uv_timer_t timer;
        uv_thread_t thread;

    protected:
        SafeQueue<ByteBufferPtr> input;

    public:
        std::function<void(ByteBufferPtr)> onSuccessCallback;
        std::function<void()> onTimeoutCallback;
    };
}

#endif