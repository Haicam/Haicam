#pragma once

#include "haicam/ByteBuffer.hpp"

namespace haicam
{
    class RSA
    {
    private:
        /* data */
    public:
        RSA(int keyLength = 128);
        ~RSA();

        bool generateKeyPair(std::string privateKeyPath, std::string publicKeyPair);
        bool loadKeyPair(std::string privateKeyPath, std::string publicKeyPair);
        
        ByteBufferPtr encrypt(ByteBufferPtr data);
        ByteBufferPtr decrypt(ByteBufferPtr data);
    };

    RSA::RSA(/* args */)
    {
    }

    RSA::~RSA()
    {
    }
}
