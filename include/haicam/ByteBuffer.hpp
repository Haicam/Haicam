#ifndef __HAICAM_BYTE_BUFFER__
#define __HAICAM_BYTE_BUFFER__

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
            std::copy((unsigned char *)data, (unsigned char *)data + length, buffer.begin());
        }

    public:
        std::vector<unsigned char> buffer;

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

        bool fillData(void *data, int length, int offset = 0)
        {
            if (offset + length > buffer.size())
            {
                return false;
            }
            std::copy((unsigned char *)data, (unsigned char *)data + length, buffer.begin() + offset);
            return true;
        }

        unsigned char * getData()
        {
            return buffer.data();
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