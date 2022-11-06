#pragma once

#include <memory>
#include <typeinfo>
#include "haicam/MacroDefs.hpp"

extern "C"
{
#include "chan/chan.h"
}

namespace haicam
{
    template <class T>
    class Chan
    {
    private:
        chan_t *pChan;
        size_t capacity;
        bool pureData;

    public:
        Chan(size_t capacity)
        {
            const std::type_info &ti1 = typeid(void *);
            const std::type_info &ti2 = typeid(T);

            this->pureData = (ti1 == ti2);
            this->capacity = capacity;

            pChan = chan_init(capacity);
        }
        ~Chan()
        {
            if (!pureData)
            {
                closeAndClean();
            }

            chan_dispose(pChan);
        }

        int sendData(void *data)
        {
            H_ASSERT(pureData);
            return chan_send(pChan, data);
        }

        int recvData(void **data)
        {
            H_ASSERT(pureData);
            return chan_recv(pChan, data);
        }

        int send(T ptr)
        {
            H_ASSERT(!pureData);

            T *wrapped = new T();
            if (!wrapped)
            {
                return -1;
            }
            *wrapped = ptr;
            int success = chan_send(pChan, (void *)wrapped);
            if (success != 0)
            {
                delete wrapped;
            }

            return success;
        }

        int operator<<(T ptr)
        {
            return send(ptr);
        }

        int recv(T &ptr)
        {
            H_ASSERT(!pureData);

            T *wrapped = NULL;
            int success = chan_recv(pChan, (void **)&wrapped);
            if (wrapped != NULL)
            {
                ptr = *wrapped;
                delete wrapped;
            }

            return success;
        }

        int operator>>(T &ptr)
        {
            return recv(ptr);
        }

        int close()
        {
            return chan_close(pChan);
        }

        int isClosed()
        {
            return chan_is_closed(pChan);
        }

        int selectSendData(void *data)
        {
            H_ASSERT(pureData);
            int success = chan_select(&pChan, 1, &data,
                                      NULL, 0, NULL);
            return success;
        }

        int selectRecvData(void *data)
        {
            H_ASSERT(pureData);
            int success = chan_select(NULL, 0, NULL,
                                      &pChan, 1, &data);
            return success;
        }

        int selectSend(T ptr)
        {
            H_ASSERT(!pureData);

            T *wrapped = new T();
            if (!wrapped)
            {
                return -1;
            }
            *wrapped = ptr;
            int success = chan_select(&pChan, 1, (void **)&wrapped,
                                      NULL, 0, NULL);
            if (success != 0)
            {
                delete wrapped;
            }
            return success;
        }

        int selectRecv(T &ptr)
        {
            H_ASSERT(!pureData);

            T *wrapped = NULL;
            int success = chan_select(NULL, 0, NULL,
                                      &pChan, 1, (void **)&wrapped);
            if (wrapped != NULL)
            {
                ptr = *wrapped;
                delete wrapped;
            }
            return success;
        }

        int size()
        {
            return chan_size(pChan);
        }

        void closeAndClean()
        {
            H_ASSERT(!pureData);

            if (!isClosed())
                close();

            if (this->capacity > 0)
            {
                while (this->size())
                {
                    T _a;
                    *this >> _a;
                }
            }
            else
            {
                T _a;
                *this >> _a;
            }
        }

        static int selectData(Chan *recvChans[], int recv_count, void **recv_out,
                              Chan *sendChans[], int send_count, void *send_msgs[])
        {
            if (recvChans != NULL)
                H_ASSERT(recvChans[0]->pureData);
            if (sendChans != NULL)
                H_ASSERT(sendChans[0]->pureData);

            chan_t *recv_chans[recv_count];
            chan_t *send_chans[send_count];

            for (int i = 0; i < recv_count; i++)
                recv_chans[i] = recvChans[i]->pChan;
            for (int i = 0; i < send_count; i++)
                send_chans[i] = sendChans[i]->pChan;

            return chan_select(recv_chans, recv_count, recv_out,
                               send_chans, send_count, send_msgs);
        }
    };

}