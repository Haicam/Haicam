#include "gtest/gtest.h"
#include "haicam/Context.hpp"
#include "haicam/Encryption.hpp"
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/aes.h>

using namespace haicam;
using namespace haicam::Encryption;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

TEST(encryption_test, rsa) {

}

TEST(encryption_test, aes) {
    std::string strKey = std::string(16, 'a');
    AES_KEY aes_key;
    unsigned char civ[AES_BLOCK_SIZE];
    memset(civ, 0x00, AES_BLOCK_SIZE);

    if(AES_set_encrypt_key((const unsigned char*)strKey.c_str(), (int)(strKey.length()*8), &aes_key) < 0)
    {
        printf("AES key error 1");
    }

    std::string plainData = std::string(35, '\x02');
    int len = AES_BLOCK_SIZE * ((plainData.length()/AES_BLOCK_SIZE) + 1);

    unsigned char enc[len];
    memset(enc, 0, len);
    AES_cbc_encrypt((const unsigned char*)plainData.c_str(), enc, plainData.length(), &aes_key, civ, AES_ENCRYPT);

    for(int j = 0; j < len; j++) printf(" %02X", enc[j]);printf("\n");

    if(AES_set_decrypt_key((const unsigned char*)strKey.c_str(), (int)(strKey.length()*8), &aes_key) < 0)
    {
        printf("AES key error 2");
    }

    memset(civ, 0x00, AES_BLOCK_SIZE);
    unsigned char dec[len];
    memset(dec, 0, len);

    AES_cbc_encrypt((const unsigned char*)enc, dec, len, &aes_key, civ, AES_DECRYPT);
    for(int j = 0; j < len; j++) printf(" %02X", dec[j]);printf("\n"); 
    // default 0 padding
    // std::string(3, '\x02') -> 02 02 02 00 00 00 00 00 00 00 00 00 00 00 00 00
    // std::string(16, '\x02') -> 02 02 02 02 02 02 02 02 02 02 02 02 02 02 02 02 01 9D 04 D0 5B D3 F0 6B 5A 6A 43 04 3D 8C 8D EB
}

#pragma GCC diagnostic pop