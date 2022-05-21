//
//  dRsaAes.cpp
//  CloudCamera
//
//  Created by Illidan on 16/3/25.
//
//

#include "CloudCameraHub/dRsaAes.h"
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/aes.h>
#include "CloudCameraHub/cocos2d.h"
extern "C" {
#include "CloudCameraHub/BoardMgr.h"
}
#include "CloudCameraHub/ABMUtils.h"

static char* EncodeCBCAESOutData = NULL;
static char* EncodeCBCAESInData = NULL;
static LinuxLock ssLock;


std::string EncodeCBCAES( const std::string& strKey, const std::string& strdata,unsigned char* cIV)
{
    AutoLinuxLock lllLock(&ssLock);
    if (EncodeCBCAESOutData == NULL) {
        EncodeCBCAESOutData = (char*)malloc(MAX_ENCODE_DATALENGTH);
        memset(EncodeCBCAESOutData, 0, MAX_ENCODE_DATALENGTH);
    }
    if (EncodeCBCAESInData == NULL) {
        EncodeCBCAESInData = (char*)malloc(MAX_ENCODE_DATALENGTH);
        memset(EncodeCBCAESInData, 0, MAX_ENCODE_DATALENGTH);
    }
    std::string strRet = "";
    std::string data_bak = strdata;
    int data_length = (unsigned int)(data_bak.length());
    int padding = 0;
    if (data_bak.length() % AES_BLOCK_SIZE > 0)
    {
        padding =  AES_BLOCK_SIZE - data_bak.length() % AES_BLOCK_SIZE;
    }
    unsigned char paddinglength = 0;
    data_length += padding;
    while (padding > 0)
    {
        data_bak += paddinglength;
        padding--;
    }
    int outLength;
//    log("CEncodeCBCAES data_length %d",data_length);
//    ssLock.Lock();
    int encodeCount = data_length/MAX_ENCODE_DATALENGTH;
    for (int i = 0; i < encodeCount; i++) {
        memcpy(EncodeCBCAESInData, data_bak.c_str()+i*MAX_ENCODE_DATALENGTH, MAX_ENCODE_DATALENGTH);
        int ret = CEncodeCBCAES(strKey.c_str(), strKey.size(), EncodeCBCAESInData, MAX_ENCODE_DATALENGTH, (char*)EncodeCBCAESOutData, &outLength, cIV);
        //    ssLock.UnLock();
//        log("CEncodeCBCAES ret %d",ret);
        if (ret >= 0) {
            strRet += std::string((char*)EncodeCBCAESOutData,MAX_ENCODE_DATALENGTH);
        }else{
            log("A CEncodeCBCAES ret %d",ret);
            return "";
        }
    }
    
    int endEncodeDataLength = data_length-encodeCount*MAX_ENCODE_DATALENGTH;
    if (endEncodeDataLength >= 16) {
        memcpy(EncodeCBCAESInData, data_bak.c_str()+encodeCount*MAX_ENCODE_DATALENGTH, endEncodeDataLength);
        int ret = CEncodeCBCAES(strKey.c_str(), strKey.size(), EncodeCBCAESInData, endEncodeDataLength, (char*)EncodeCBCAESOutData, &outLength, cIV);
        //    ssLock.UnLock();
//        log("CEncodeCBCAES ret %d",ret);
        if (ret >= 0) {
            strRet += std::string((char*)EncodeCBCAESOutData,endEncodeDataLength);
        }else {
            log("B CEncodeCBCAES ret %d endEncodeDataLength %d",ret,endEncodeDataLength);
            return "";
        }
//        log("####strRet");
    }
//    usleep(100);
//    munmap((char*)wrq.DataIn_addr, dma_size);
    return strRet;
    }

std::string EncodeAES( const std::string& password, const std::string& data , bool bIsComplement)
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
//    char paddinglength = 0;
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
//        AES_encrypt((const unsigned char*)str16.c_str(), out, &aes_key);
        AES_cbc_encrypt((const unsigned char*)str16.c_str(), out, str16.length(), &aes_key, civ, AES_ENCRYPT);
        strRet += std::string((const char*)out, AES_BLOCK_SIZE);
    }
    return strRet;
}

std::string DecodeAES( const std::string& password, const std::string& strData , bool bIsComplement)
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
//        AES_decrypt((const unsigned char*)str16.c_str(), out, &aes_key);
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



std::string DecodeCBCAES( const std::string& strKey, const std::string& strData,unsigned char* cIV)
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
//        AES_decrypt((const unsigned char*)str16.c_str(), out, &aes_key);
        AES_cbc_encrypt((const unsigned char*)str16.c_str(), out, str16.length(), &aes_key, cIV, AES_DECRYPT);
        strRet += std::string((const char*)out, AES_BLOCK_SIZE);
//        for (int i = 0; i < 16; i++) {
//            printf("%2x",cIV[i]);
//        }
//        printf("\n");
    }
    return strRet;
}



