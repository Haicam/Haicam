#include "haicam/Encryption.hpp"
#include "haicam/FrameCommand.hpp"
#include "haicam/UserDefault.hpp"
#include "haicam/MacroDefs.hpp"
#include "haicam/Utils.hpp"
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/aes.h>
#include <memory>

using namespace haicam;

/*
 AES_encrypt is a software only implementation. It will never use hardware acceleration.
 Also, the OpenSSL project tells you don't use AES_encrypt and friends. Rather, they tell you to use EVP_encrypt and friends.
 cat /proc/cpuinfo
 processor    : 0
 model name    : ARMv7 Processor rev 0 (v7l)
 BogoMIPS    : 76.32
 Features    : half thumb fastmult vfp edsp neon vfpv3 tls vfpv4 idiva idivt vfpd32 lpae evtstrm aes pmull sha1 sha2 crc32
*/

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

std::string Encryption::EncodeRSAData(const std::string &strPublicKey, const std::string &strData, int keyBytes, int paddingMode)
{
    std::string strEncodeData = "";
    std::string strResoureData = strData;
    if (strData == "")
    {
        return "";
    }
    int blockSize = keyBytes;
    if (paddingMode == 1)
    {
        blockSize -= RSA_PKCS1_PADDING_SIZE;
    }
    bool bIsBreak = false;
    while (!bIsBreak)
    {

        char strSub[blockSize];
        memset(strSub, 0, blockSize);
        if (strResoureData.length() > blockSize)
        {
            memcpy(strSub, strResoureData.c_str(), blockSize);
            strEncodeData += EncodeRSABlock(strPublicKey, strSub, blockSize, paddingMode);
            strResoureData = strResoureData.substr(blockSize);
        }
        else
        {
            memcpy(strSub, strResoureData.c_str(), strResoureData.size());
            strEncodeData += EncodeRSABlock(strPublicKey, strSub, strResoureData.size(), paddingMode);
            bIsBreak = true;
        }
    }

    return strEncodeData;
}

std::string Encryption::DecodeRSAData(const std::string &strPrivateKey, const std::string &strData, int keyBytes, int paddingMode)
{
    std::string strEncodeData = "";
    std::string strResoureData = strData;
    if (strData == "")
    {
        return "";
    }
    bool bIsBreak = false;
    while (!bIsBreak)
    {

        char strSub[keyBytes];
        memset(strSub, 0, keyBytes);
        if (strResoureData.length() > keyBytes)
        {
            memcpy(strSub, strResoureData.c_str(), keyBytes);
            strEncodeData += DecodeRSABlock(strPrivateKey, strSub, keyBytes, paddingMode);
            strResoureData = strResoureData.substr(keyBytes);
        }
        else
        {
            memcpy(strSub, strResoureData.c_str(), strResoureData.size());
            strEncodeData += DecodeRSABlock(strPrivateKey, strSub, strResoureData.size(), paddingMode);
            bIsBreak = true;
        }
    }

    return strEncodeData;
}

std::string Encryption::EncodeRSABlock(const std::string &strPublicKey, const char *strData, int len, int paddingMode)
{
    if (strPublicKey.empty() || !strData)
    {
        fprintf(stderr, "Error EncodeRSABlock 1\n");
        return "";
    }

    std::string strRet;
    BIO *pRSAPublicKeyBio = NULL;
    RSA *pRSAPublicKey = NULL;
    char *chPublicKey = const_cast<char *>(strPublicKey.c_str());
    if ((pRSAPublicKeyBio = BIO_new_mem_buf(chPublicKey, -1)) == NULL)
    {
        fprintf(stderr, "Error EncodeRSABlock 2\n");
        return "";
    }
    pRSAPublicKey = PEM_read_bio_RSA_PUBKEY(pRSAPublicKeyBio, NULL, NULL, NULL);
    if (!pRSAPublicKey)
    {
        fprintf(stderr, "Error EncodeRSABlock %s\n", strPublicKey.c_str());
        fprintf(stderr, "Error EncodeRSABlock 3\n");
        return "";
    }

    int nLen = RSA_size(pRSAPublicKey);
    char *pEncode = new char[nLen + 1];
    int ret = RSA_public_encrypt(len, (const unsigned char *)strData, (unsigned char *)pEncode, pRSAPublicKey, paddingMode == 0 ? RSA_NO_PADDING : RSA_PKCS1_PADDING);
    if (ret >= 0)
    {
        strRet = std::string(pEncode, ret);
    }
    else
    {
        char err[128];
        ERR_load_crypto_strings();
        ERR_error_string(ERR_get_error(), err);
        fprintf(stderr, "Error encrypting message: %s\n", err);
    }
    delete[] pEncode;
    BIO_free_all(pRSAPublicKeyBio);
    RSA_free(pRSAPublicKey);

    return strRet;
}

std::string Encryption::DecodeRSABlock(const std::string &strPrivateKey, const char *strData, int len, int paddingMode)
{
    if (strPrivateKey.empty() || !strData)
    {
        return "";
    }

    std::string strRet = "";
    BIO *pRSAPrivateKeyBio = NULL;
    RSA *pRSAPrivateKey = NULL;
    char *chPrivateKey = const_cast<char *>(strPrivateKey.c_str());
    if ((pRSAPrivateKeyBio = BIO_new_mem_buf(chPrivateKey, -1)) == NULL)
    {
        return "";
    }
    pRSAPrivateKey = PEM_read_bio_RSAPrivateKey(pRSAPrivateKeyBio, NULL, NULL, NULL);
    if (!pRSAPrivateKey)
    {
        return "";
    }

    int nLen = RSA_size(pRSAPrivateKey);
    char *pDecode = new char[nLen + 1];

    int ret = RSA_private_decrypt(len, (const unsigned char *)strData, (unsigned char *)pDecode, pRSAPrivateKey, paddingMode == 0 ? RSA_NO_PADDING : RSA_PKCS1_PADDING);
    if (ret >= 0)
    {
        strRet = std::string((char *)pDecode, ret);
    }
    else
    {
        char err[128];
        ERR_load_crypto_strings();
        ERR_error_string(ERR_get_error(), err);
        fprintf(stderr, "Error decrypting message: %s\n", err);
    }

    delete[] pDecode;

    BIO_free_all(pRSAPrivateKeyBio);

    RSA_free(pRSAPrivateKey);

    return strRet;
}

std::string Encryption::EncodeAES(const std::string &strKey, const std::string &data, bool bIsComplement)
{
    AES_KEY aes_key;
    if (AES_set_encrypt_key((const unsigned char *)strKey.c_str(), (int)(strKey.length() * 8), &aes_key) < 0)
    {
        return "";
    }
    std::string strRet;
    unsigned char civ[AES_BLOCK_SIZE];
    memset(civ, 0x00, AES_BLOCK_SIZE);
    std::string data_bak = data;
    unsigned int data_length = (unsigned int)(data_bak.length());
    int padding = 0;
    if (data_bak.length() % AES_BLOCK_SIZE > 0)
    {
        padding = AES_BLOCK_SIZE - data_bak.length() % AES_BLOCK_SIZE;
    }
    if (bIsComplement)
    {
        if (padding == 0)
        {
            padding = AES_BLOCK_SIZE;
        }
    }
    char paddinglength = (char)padding;

    data_length += padding;
    while (padding > 0)
    {
        data_bak += paddinglength;
        padding--;
    }
    for (unsigned int i = 0; i < data_length / AES_BLOCK_SIZE; i++)
    {
        std::string str16 = data_bak.substr(i * AES_BLOCK_SIZE, AES_BLOCK_SIZE);
        unsigned char out[AES_BLOCK_SIZE];
        memset(out, 0, AES_BLOCK_SIZE);

        // can do mutiple AES_BLOCK_SIZE, and default 0 padding
        AES_cbc_encrypt((const unsigned char *)str16.c_str(), out, str16.length(), &aes_key, civ, AES_ENCRYPT);
        strRet += std::string((const char *)out, AES_BLOCK_SIZE);
    }
    return strRet;
}

std::string Encryption::DecodeAES(const std::string &strKey, const std::string &strData, bool bIsComplement)
{
    AES_KEY aes_key;
    if (AES_set_decrypt_key((const unsigned char *)(strKey.c_str()), (int)(strKey.length() * 8), &aes_key) < 0)
    {
        return "";
    }
    std::string strRet;
    unsigned char civ[AES_BLOCK_SIZE];
    memset(civ, 0x00, AES_BLOCK_SIZE);
    int strDataAESLength = (int)(strData.length() / AES_BLOCK_SIZE);
    for (unsigned int i = 0; i < strData.length() / AES_BLOCK_SIZE; i++)
    {
        std::string str16 = strData.substr(i * AES_BLOCK_SIZE, AES_BLOCK_SIZE);
        unsigned char out[AES_BLOCK_SIZE];
        memset(out, 0, AES_BLOCK_SIZE);

        // can do mutiple AES_BLOCK_SIZE
        AES_cbc_encrypt((const unsigned char *)str16.c_str(), out, str16.length(), &aes_key, civ, AES_DECRYPT);
        if (bIsComplement && i == strDataAESLength - 1)
        {
            if (out[AES_BLOCK_SIZE - 1] < AES_BLOCK_SIZE)
            {
                strRet += std::string((const char *)out, AES_BLOCK_SIZE - out[AES_BLOCK_SIZE - 1]);
            }
        }
        else
        {
            strRet += std::string((const char *)out, AES_BLOCK_SIZE);
        }
    }
    return strRet;
}

std::string Encryption::EncodeCBCAES(const std::string &strKey, const std::string &strdata, unsigned char *cIV)
{
    std::string strRet = "";

    AES_KEY aes_key;
    if (AES_set_encrypt_key((const unsigned char *)strKey.c_str(), (int)(strKey.length() * 8), &aes_key) < 0)
    {
        return "";
    }

    std::string data_bak = strdata;
    unsigned int data_length = (unsigned int)(data_bak.length());
    int padding = 0;
    if (data_bak.length() % AES_BLOCK_SIZE > 0)
    {
        padding = AES_BLOCK_SIZE - data_bak.length() % AES_BLOCK_SIZE;
        data_length += padding;
    }

    for (unsigned int i = 0; i < data_length / AES_BLOCK_SIZE; i++)
    {
        if (i == (data_length / AES_BLOCK_SIZE - 1))
        {
            std::string str16 = data_bak.substr(i * AES_BLOCK_SIZE, AES_BLOCK_SIZE - padding);
            unsigned char out[AES_BLOCK_SIZE];
            memset(out, 0, AES_BLOCK_SIZE);
            AES_cbc_encrypt((const unsigned char *)str16.c_str(), out, str16.length(), &aes_key, cIV, AES_ENCRYPT);
            strRet += std::string((const char *)out, AES_BLOCK_SIZE);
        }
        else
        {
            std::string str16 = data_bak.substr(i * AES_BLOCK_SIZE, AES_BLOCK_SIZE);
            unsigned char out[AES_BLOCK_SIZE];
            memset(out, 0, AES_BLOCK_SIZE);
            AES_cbc_encrypt((const unsigned char *)str16.c_str(), out, str16.length(), &aes_key, cIV, AES_ENCRYPT);
            strRet += std::string((const char *)out, AES_BLOCK_SIZE);
        }
    }

    return strRet;
}

std::string Encryption::DecodeCBCAES(const std::string &strKey, const std::string &strData, unsigned char *cIV)
{
    AES_KEY aes_key;
    if (AES_set_decrypt_key((const unsigned char *)strKey.c_str(), (int)(strKey.length() * 8), &aes_key) < 0)
    {
        return "";
    }

    std::string strRet;
    int strDataAESLength = (int)(strData.length() / AES_BLOCK_SIZE);
    for (unsigned int i = 0; i < strDataAESLength; i++)
    {
        std::string str16 = strData.substr(i * AES_BLOCK_SIZE, AES_BLOCK_SIZE);
        unsigned char out[AES_BLOCK_SIZE];
        memset(out, 0, AES_BLOCK_SIZE);

        AES_cbc_encrypt((const unsigned char *)str16.c_str(), out, str16.length(), &aes_key, cIV, AES_DECRYPT);
        strRet += std::string((const char *)out, AES_BLOCK_SIZE);
    }
    return strRet;
}

bool Encryption::generateRSAKeyPair(std::string &strPublicKey, std::string &strPrivateKey, int keyBits)
{
    int ret = 0;
    RSA *r = NULL;
    BIGNUM *bne = NULL;
    BIO *bp_public = NULL;
    BIO *bp_private = NULL;
    unsigned long e = RSA_F4;

    bne = BN_new();
    ret = BN_set_word(bne, e);
    if (ret != 1)
    {
        BN_free(bne);
        return false;
    }

    r = RSA_new();
    ret = RSA_generate_key_ex(r, keyBits, bne, NULL);
    if (ret != 1)
    {
        RSA_free(r);
        BN_free(bne);
        return false;
    }

    bp_private = BIO_new(BIO_s_mem());
    ret = PEM_write_bio_RSAPrivateKey(bp_private, r, NULL, NULL, 0, NULL, NULL);
    size_t pri_len = BIO_pending(bp_private);
    H_MEM_SP(char, privateKeyBuffer, pri_len);
    BIO_read(bp_private, privateKeyBuffer.get(), pri_len);
    strPrivateKey = std::string(privateKeyBuffer.get(), pri_len);

    if (ret != 1)
    {
        BIO_free_all(bp_private);
        RSA_free(r);
        BN_free(bne);
        return false;
    }

    bp_public = BIO_new(BIO_s_mem());
    ret = PEM_write_bio_RSA_PUBKEY(bp_public, r);
    size_t pub_len = BIO_pending(bp_public);
    H_MEM_SP(char, publicKeyBuffer, pub_len);
    BIO_read(bp_public, publicKeyBuffer.get(), pub_len);
    strPublicKey = std::string(publicKeyBuffer.get(), pub_len);

    if (ret != 1)
    {
        BIO_free_all(bp_public);
        BIO_free_all(bp_private);
        RSA_free(r);
        BN_free(bne);
        return false;
    }

    BIO_free_all(bp_public);
    BIO_free_all(bp_private);
    RSA_free(r);
    BN_free(bne);
    return true;
}

#pragma GCC diagnostic pop

std::string Encryption::packFrame(uint32 frameNum, uint8 cmd, uint8 cmdType, std::string payload, uint32 remoteAddr, uint8 encryptType, std::string masterKey)
{
    std::string data;

    data.append(1, FRAME_CMD_SOF);
    data.append(1, encryptType);

    std::string strpayloadStr = "";

    std::string data2;
    data2.append(Utils::uint32ToNetworkString(frameNum));
    data2.append(Utils::uint32ToNetworkString(UserDefault::getInstance()->getLocalAddr()));
    data2.append(Utils::uint32ToNetworkString(remoteAddr));
    data2.append(1, cmd);
    data2.append(1, cmdType);
    uint32 len = (uint32)payload.length();
    data2.append(Utils::uint32ToNetworkString(len));

    data2.append(payload);

    if (encryptType == FRAME_RSA_1024)
    {
        strpayloadStr = EncodeRSAData(UserDefault::getInstance()->getRSAKey1024(), data2, 128);
    }
    else if (encryptType == FRAME_AES_128)
    {
        strpayloadStr = EncodeAES(UserDefault::getInstance()->getAESKey128(), data2);
    }
    else if (encryptType == FRAME_RSA_2048)
    {
        strpayloadStr = EncodeRSAData(UserDefault::getInstance()->getRSAKey2048(), data2, 256);
    }
    else if (encryptType == FRAME_AES_256)
    {
        strpayloadStr = EncodeAES(UserDefault::getInstance()->getAESKey256(), data2);
    }
    else if (encryptType == FRAME_SSL_RSA_2048)
    {
        strpayloadStr = EncodeRSAData(UserDefault::getInstance()->getServerRSAKey2048(), data2, 256, 1);
    }
    else if (encryptType == FRAME_SSL_AES_256)
    {
        strpayloadStr = EncodeAES(masterKey, data2);
    }
    else if (encryptType == FRAME_PLAIN)
    {
        strpayloadStr = data2;
    }
    else
    {
        H_ASSERT_ERR_STR("Not supported encryption type");
        return "";
    }

    if (strpayloadStr == "")
    {
        return "";
    }

    data.append(Utils::uint32ToNetworkString((uint32)strpayloadStr.length()));
    data.append(strpayloadStr);
    uint8 checksum = 0xFF;

    for (int i = 1; i < data.length(); i++)
    {
        checksum ^= data[i];
    }
    data.append(1, checksum);

    return data;
}

bool Encryption::unpackFrame(uint8& cmd, uint8& cmdType, uint32& fromAddr, uint32& frameNum, std::string &payload, std::string &lastBuffer, std::string& buffer, std::string masterKey)
{
    if (lastBuffer.empty())
    {
        if ((uint8)buffer[0] != FRAME_CMD_SOF && (uint8)buffer[0] != FRAME_SIGN_HEART_BEAT)
        {
            return false;
        }
    }
    else
    {
        buffer = lastBuffer.append(buffer);
    }

    while (1)
    {
        if (buffer.size() > 0)
        {
            if ((uint8)buffer[0] == FRAME_SIGN_HEART_BEAT)
            {
                buffer = buffer.substr(1);
                continue;
            }
            if ((uint8)buffer[0] != FRAME_CMD_SOF)
            {
                lastBuffer = "";
                return false;
            }
        }

        if (buffer.length() < heardSize + 1)
        {
            break;
        }

        uint32 len = Utils::networkStringToUint32(buffer, heardSize - 4);

        H_ASSERT(len < 1 * 1024 * 1024);

        if (buffer.length() < len + heardSize + 1)
        {
            break;
        }

        uint8 checksum = 0xFF;
        for (int i = 1; i < len + heardSize; i++)
        {
            checksum ^= buffer[i];
        }

        if (checksum != (uint8)buffer[len + heardSize])
        {
            lastBuffer = "";
            return false;
        }

        std::string strframe = buffer.substr(heardSize, len);

        if ((uint8)buffer[1] == FRAME_RSA_1024)
        {
            strframe = DecodeRSAData(UserDefault::getInstance()->getRSAKey1024(), strframe, 128);
        }
        else if ((uint8)buffer[1] == FRAME_AES_128)
        {
            strframe = DecodeAES(UserDefault::getInstance()->getAESKey128(), strframe);
        }
        else if ((uint8)buffer[1] == FRAME_RSA_2048)
        {
            strframe = DecodeRSAData(UserDefault::getInstance()->getRSAKey2048(), strframe, 256);
        }
        else if ((uint8)buffer[1] == FRAME_AES_256)
        {
            strframe = DecodeAES(UserDefault::getInstance()->getAESKey256(), strframe);
        }
        else if ((uint8)buffer[1] == FRAME_SSL_RSA_2048)
        {
            strframe = DecodeRSAData(UserDefault::getInstance()->getServerRSAKey2048(), strframe, 256, 1);
        }
        else if ((uint8)buffer[1] == FRAME_SSL_AES_256)
        {
            strframe = DecodeAES(masterKey, strframe);
        }
        else if ((uint8)buffer[1] == FRAME_PLAIN)
        {
            // plain data
        }
        else
        {
            H_ASSERT_WARN_STR("Not supported encryption type");
            lastBuffer = "";
            return false;
        }

        if (strframe.size() > heardSize2)
        {
            frameNum = Utils::networkStringToUint32(strframe, 0);
            fromAddr = Utils::networkStringToUint32(strframe, 4);
            cmd = (uint8)strframe[heardSize2 - 6];
            cmdType = (uint8)strframe[heardSize2 - 5];
            payload = strframe.substr(heardSize2, strframe.length() - heardSize2);

            if (cmdType == FRAME_CMD_REQ || cmdType == FRAME_CMD_RES)
            {
                buffer = buffer.substr(len + heardSize + 1);
                return true;
            }
            else
            {
                H_ASSERT_WARN_STR("Not supported type");
                lastBuffer = "";
                return false;
            }
        }

        buffer = buffer.substr(len + heardSize + 1);
    }

    lastBuffer = buffer;
    return false;
}