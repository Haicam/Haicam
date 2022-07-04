#ifndef __HAICAM_BYTE_BUFFER__
#define __HAICAM_BYTE_BUFFER__

#include "haicam/Context.hpp"
#include <vector>
#include <memory>

namespace haicam
{
    class ByteBuffer;
    typedef std::shared_ptr<ByteBuffer> ByteBufferPtr;

    class ByteBuffer : public std::enable_shared_from_this<ByteBuffer>
    {
    private:
        ByteBuffer(int length) : buffer(length)
        {
        }

        ByteBuffer(void *data, int length) : buffer(length)
        {
            std::copy((char *)data, (char *)data + length, buffer.begin());
        }

    public:
        std::vector<char> buffer;

        ByteBufferPtr getPtr()
        {
            return shared_from_this();
        }

        static ByteBufferPtr create(int length)
        {

            return ByteBufferPtr(new ByteBuffer(length));
        }

        static ByteBufferPtr create(void *data, int length)
        {
            return ByteBufferPtr(new ByteBuffer(data, length));
        }

        static ByteBufferPtr create(std::string str)
        {

            return ByteBufferPtr(new ByteBuffer((void*)str.data(), str.size()));
        }

        void appendData(char* data, int length)
        {
            buffer.insert(buffer.end(), data, data + length);
        }

        bool fillData(char *data, int length, int offset = 0)
        {
            H_ASSERT(offset + length <= buffer.size());

            if (offset + length > buffer.size())
            {
                return false;
            }
            std::copy(data, data + length, buffer.begin() + offset);
            return true;
        }

        char * getDataPtr()
        {
            return buffer.data();
        }

        std::string toString()
        {
            return std::string(buffer.begin(), buffer.end());
        }

        size_t getLength()
        {
            return buffer.size();
        }

        ~ByteBuffer()
        {
        }
    };

}

#endif