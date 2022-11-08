#pragma once
#include <mutex>
#include <list>
#include <memory>

namespace haicam
{
    template <typename T_Observer, typename T_Data>
    class Observable
    {
    public:
        ~Observable()
        {
            std::lock_guard<std::mutex> lock(mtx);

            if(m_observers.size() > 0) m_observers.clear();
        }

        void addObserver(std::shared_ptr<T_Observer> observer)
        {
            std::lock_guard<std::mutex> lock(mtx);

            m_observers.push_back(observer);
        }

        void removeObserver(std::shared_ptr<T_Observer> observer)
        {
            std::lock_guard<std::mutex> lock(mtx);

            typename std::list<std::shared_ptr<T_Observer>>::iterator it = m_observers.begin();
            while (it != m_observers.end())
            {
                if (*it == observer)
                {
                    m_observers.erase(it++); // alternatively, i = m_observers.erase(i);
                }
                else
                {
                    ++ it;
                }
            }
        }

        void notify(void (T_Observer::*onData)(T_Data), T_Data data)
        {
            std::lock_guard<std::mutex> lock(mtx);

            typename std::list<std::shared_ptr<T_Observer>>::iterator it = m_observers.begin();
            while (it != m_observers.end())
            {
                ((*it).get()->*onData)(data);
                ++ it;
            }
        }

        void notify(void (T_Observer::*onData)(T_Data, size_t), T_Data data, size_t size)
        {
            std::lock_guard<std::mutex> lock(mtx);

            typename std::list<std::shared_ptr<T_Observer>>::iterator it = m_observers.begin();
            while (it != m_observers.end())
            {
                ((*it).get()->*onData)(data, size);
                ++ it;
            }
        }

    private:
        std::mutex mtx;
        std::list<std::shared_ptr<T_Observer>> m_observers;
    };

}