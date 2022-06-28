#ifndef __HAICAM_SAFE_QUEUE_HPP__
#define __HAICAM_SAFE_QUEUE_HPP__

#include <queue>
#include <mutex>
#include <condition_variable>

namespace haicam
{

    template <class T>
    class SafeQueue
    {
    public:
        SafeQueue()
            : q(), m(), c(), isClosed(false)
        {
        }

        ~SafeQueue()
        {
        }

        void enqueue(T t)
        {
            std::lock_guard<std::mutex> lock(m);
            q.push(std::move(t));
            c.notify_one();
        }

        bool dequeue(T &t)
        {
            std::lock_guard<std::mutex> lock(m);
            if (q.empty())
            {
                return false;
            }
            t = std::move(q.front());
            q.pop();
            return true;
        }

        bool dequeueWait(T &t)
        {
            std::unique_lock<std::mutex> lock(m);
            while (q.empty() && !isClosed)
            {
                c.wait(lock);
            }
            if (isClosed)
                return false;
            t = std::move(q.front());
            q.pop();
            return true;
        }

        bool isOpen()
        {
            std::unique_lock<std::mutex> lock(m);
            return !isClosed;
        }

        void close()
        {
            std::unique_lock<std::mutex> lock(m);
            std::queue<T> tmp;
            q.swap(tmp);
            isClosed = true;
            c.notify_all();
        }

    private:
        std::queue<T> q;
        mutable std::mutex m;
        std::condition_variable c;
        bool isClosed;
    };
}
#endif