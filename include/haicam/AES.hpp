#pragma once

namespace haicam
{
    class AES
    {
    private:
        /* data */
    public:
        AES(ByteBufferPtr key);
        ~AES();

        ByteBufferPtr encrypt(ByteBufferPtr data);
        ByteBufferPtr decrypt(ByteBufferPtr data);

        ByteBufferPtr encryptCBC(ByteBufferPtr data);
        ByteBufferPtr decryptCBC(ByteBufferPtr data);
    };

    AES::AES(/* args */)
    {
    }

    AES::~AES()
    {
    }
}
