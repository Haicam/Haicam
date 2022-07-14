#include "haicam/Encryption.hpp"
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/aes.h>

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

int Encryption::ENCODE_DATA_LENGTH = 128;
int Encryption::DECODE_DATA_LENGTH = 128;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

std::string Encryption::EncodeRSAData( const std::string& strPublicKey, const std::string& strData ,int length)
{
    std::string strEncodeData = "";
    std::string strResourData = strData;
    if (strData == "") {
        return "";
    }
    bool bIsBreak = false;
    while (!bIsBreak) {
        
        char strSub[ENCODE_DATA_LENGTH+1];
        memset(strSub, 0, ENCODE_DATA_LENGTH+1);
        if (strResourData.length() > length)
        {
            memcpy(strSub, strResourData.substr(0,length).c_str(), length);
            strResourData = strResourData.substr(length,(strResourData.size()-1));
        }
        else
        {
            memcpy(strSub, strResourData.c_str(), strResourData.size());
            bIsBreak = true;
        }
        strEncodeData += EncodeRSAKeyFile(strPublicKey, strSub,ENCODE_DATA_LENGTH);
    }
    
    return strEncodeData;
}

std::string Encryption::DecodeRSAData( const std::string& strPrivateKey, const std::string& strData ,int length)
{
    std::string strEncodeData = "";
    std::string strResourData = strData;
    if (strData == "") {
        return "";
    }
    bool bIsBreak = false;
    while (!bIsBreak) {
        
        char strSub[DECODE_DATA_LENGTH];
        memset(strSub, 0, DECODE_DATA_LENGTH);
        if (strResourData.length() > length)
        {
            memcpy(strSub, strResourData.substr(0,length).c_str(), length);
            strResourData = strResourData.substr(length,(strResourData.size()-1));
        }
        else
        {
            memcpy(strSub, strResourData.c_str(), strResourData.size());
            bIsBreak = true;
        }

        strEncodeData += DecodeRSAKeyFile(strPrivateKey, strSub,DECODE_DATA_LENGTH);
    }
    
    return strEncodeData;

}

std::string Encryption::EncodeRSAKeyFile( const std::string& strPublicKey, const char* strData ,int len)
{
    if (strPublicKey.empty() || !strData)
    {
        return "";
    }
    
    std::string strRet;
    BIO *pRSAPublicKeyBio = NULL;
    RSA *pRSAPublicKey = NULL;
    char *chPublicKey = const_cast<char *>(strPublicKey.c_str());
    if ((pRSAPublicKeyBio = BIO_new_mem_buf(chPublicKey, -1)) == NULL)
    {
        return "";
    }
    pRSAPublicKey = PEM_read_bio_RSA_PUBKEY(pRSAPublicKeyBio, NULL, NULL, NULL);
    if(!pRSAPublicKey)
    {
        return "";
    }
    
    int nLen = RSA_size(pRSAPublicKey);
    char* pEncode = new char[nLen + 1];
    int ret = RSA_public_encrypt(len, (const unsigned char*)strData, (unsigned char*)pEncode, pRSAPublicKey, RSA_NO_PADDING);
    if (ret >= 0)
    {
        strRet = std::string(pEncode, ret);
    }else{
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

std::string Encryption::DecodeRSAKeyFile( const std::string& strPrivateKey, const char* strData ,int len)
{
    if (strPrivateKey.empty() || !strData)
    {
        return "";
    }
    
    std::string strRet="";
    BIO *pRSAPrivateKeyBio = NULL;
    RSA *pRSAPrivateKey = NULL;
    char *chPrivateKey = const_cast<char *>(strPrivateKey.c_str());
    if ((pRSAPrivateKeyBio = BIO_new_mem_buf(chPrivateKey, -1)) == NULL)
    {
        return "";
    }
    pRSAPrivateKey = PEM_read_bio_RSAPrivateKey(pRSAPrivateKeyBio, NULL, NULL, NULL);
    if(!pRSAPrivateKey)
    {
        return "";
    }
    
    int nLen = RSA_size(pRSAPrivateKey);
    char* pDecode = new char[nLen+1];

    int ret = RSA_private_decrypt(len, (const unsigned char*)strData, (unsigned char*)pDecode, pRSAPrivateKey, RSA_NO_PADDING);
    if(ret >= 0)
    {
        strRet = std::string((char*)pDecode, ret);
    }else{
        char err[128];
        ERR_load_crypto_strings();
        ERR_error_string(ERR_get_error(), err);
        fprintf(stderr, "Error decrypting message: %s\n", err);

        strRet="";
    }

    delete [] pDecode;

    BIO_free_all(pRSAPrivateKeyBio);

    RSA_free(pRSAPrivateKey);

    return strRet;
}

std::string Encryption::EncodeAES( const std::string& password, const std::string& data , bool bIsComplement)
{
    AES_KEY aes_key;
    if(AES_set_encrypt_key((const unsigned char*)password.c_str(), (int)(password.length()*8), &aes_key) < 0)
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
        padding =  AES_BLOCK_SIZE - data_bak.length() % AES_BLOCK_SIZE;
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
    for(unsigned int i = 0; i < data_length/AES_BLOCK_SIZE; i++)
    {
        std::string str16 = data_bak.substr(i*AES_BLOCK_SIZE, AES_BLOCK_SIZE);
        unsigned char out[AES_BLOCK_SIZE];
        memset(out, 0, AES_BLOCK_SIZE);

        AES_cbc_encrypt((const unsigned char*)str16.c_str(), out, str16.length(), &aes_key, civ, AES_ENCRYPT);
        strRet += std::string((const char*)out, AES_BLOCK_SIZE);
    }
    return strRet;
}

std::string Encryption::DecodeAES( const std::string& password, const std::string& strData , bool bIsComplement)
{
    AES_KEY aes_key;
    if(AES_set_decrypt_key((const unsigned char*)(password.c_str()),(int)(password.length()*8), &aes_key) < 0)
    {
        return "";
    }
    std::string strRet;
    unsigned char civ[AES_BLOCK_SIZE];
    memset(civ, 0x00, AES_BLOCK_SIZE);
    int strDataAESLength = (int)(strData.length()/AES_BLOCK_SIZE);
    for(unsigned int i = 0; i < strData.length()/AES_BLOCK_SIZE; i++)
    {
        std::string str16 = strData.substr(i*AES_BLOCK_SIZE, AES_BLOCK_SIZE);
        unsigned char out[AES_BLOCK_SIZE];
        memset(out, 0, AES_BLOCK_SIZE);

        AES_cbc_encrypt((const unsigned char*)str16.c_str(), out, str16.length(), &aes_key, civ, AES_DECRYPT);
        if (bIsComplement && i == strDataAESLength-1)
        {
            if (out[AES_BLOCK_SIZE-1] < AES_BLOCK_SIZE)
            {
                strRet += std::string((const char*)out, AES_BLOCK_SIZE-out[AES_BLOCK_SIZE-1]);
            }
        }
        else
        {
            strRet += std::string((const char*)out, AES_BLOCK_SIZE);
        }
    }
    return strRet;
}


std::string Encryption::EncodeCBCAES( const std::string& strKey, const std::string& strdata,unsigned char* cIV)
{
    std::string strRet = "";


    AES_KEY aes_key;
    if(AES_set_encrypt_key((const unsigned char*)strKey.c_str(), (int)(strKey.length()*8), &aes_key) < 0)
    {
        return "";
    }

    std::string data_bak = strdata;
    unsigned int data_length = (unsigned int)(data_bak.length());
    int padding = 0;
    if (data_bak.length() % AES_BLOCK_SIZE > 0)
    {
        padding =  AES_BLOCK_SIZE - data_bak.length() % AES_BLOCK_SIZE;
        data_length += padding;
    }

    for(unsigned int i = 0; i < data_length/AES_BLOCK_SIZE; i++)
    {
        if (i == (data_length/AES_BLOCK_SIZE -1 ))
        {
            std::string str16 = data_bak.substr(i*AES_BLOCK_SIZE, AES_BLOCK_SIZE-padding);
            unsigned char out[AES_BLOCK_SIZE];
            memset(out, 0, AES_BLOCK_SIZE);
            AES_cbc_encrypt((const unsigned char*)str16.c_str(), out, str16.length(), &aes_key, cIV, AES_ENCRYPT);
            strRet += std::string((const char*)out, AES_BLOCK_SIZE);
        }
        else
        {
            std::string str16 = data_bak.substr(i*AES_BLOCK_SIZE, AES_BLOCK_SIZE);
            unsigned char out[AES_BLOCK_SIZE];
            memset(out, 0, AES_BLOCK_SIZE);
            AES_cbc_encrypt((const unsigned char*)str16.c_str(), out, str16.length(), &aes_key, cIV, AES_ENCRYPT);
            strRet += std::string((const char*)out, AES_BLOCK_SIZE);
        }
        
    }

    return strRet;
}

std::string Encryption::DecodeCBCAES( const std::string& strKey, const std::string& strData,unsigned char* cIV)
{
    AES_KEY aes_key;
    if(AES_set_decrypt_key((const unsigned char*)strKey.c_str(), (int)(strKey.length()*8), &aes_key) < 0)
    {
        return "";
    }
    
    std::string strRet;
    int strDataAESLength = (int)(strData.length()/AES_BLOCK_SIZE);
    for(unsigned int i = 0; i < strDataAESLength; i++)
    {
        std::string str16 = strData.substr(i*AES_BLOCK_SIZE, AES_BLOCK_SIZE);
        unsigned char out[AES_BLOCK_SIZE];
        memset(out, 0, AES_BLOCK_SIZE);

        AES_cbc_encrypt((const unsigned char*)str16.c_str(), out, str16.length(), &aes_key, cIV, AES_DECRYPT);
        strRet += std::string((const char*)out, AES_BLOCK_SIZE);
    }
    return strRet;
}


bool Encryption::generateKey(std::string PRIKeyPath,std::string PubKeyPath)
{
    int             ret = 0;
    RSA             *r = NULL;
    BIGNUM          *bne = NULL;
    BIO             *bp_public = NULL;
    BIO             *bp_private = NULL;
    int             bits = 1024;
    unsigned long   e = RSA_F4;
    
    bne = BN_new();
    ret = BN_set_word(bne,e);
    if (ret != 1) {
        BN_free(bne);
        return false;
    }
    
    r = RSA_new();
    ret = RSA_generate_key_ex(r, bits, bne, NULL);
    if (ret != 1) {
        RSA_free(r);
        BN_free(bne);
        return false;
    }
    
    bp_private = BIO_new_file(PRIKeyPath.c_str(), "wb+");
    ret = PEM_write_bio_RSAPrivateKey(bp_private, r, NULL, NULL, 0, NULL, NULL);
    if(ret != 1){
        BIO_free_all(bp_private);
        RSA_free(r);
        BN_free(bne);
        return false;
    }
    
    bp_public = BIO_new_file(PubKeyPath.c_str(), "wb+");
    ret = PEM_write_bio_RSA_PUBKEY(bp_public, r);
    if(ret != 1){
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