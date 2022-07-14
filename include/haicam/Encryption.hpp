#pragma once
#include <string>

namespace haicam{

    namespace Encryption
    {
        extern int ENCODE_DATA_LENGTH;
        extern int DECODE_DATA_LENGTH;

        bool generateKey(std::string PRIKeyPath,std::string PubKeyPath);

        std::string EncodeRSAData( const std::string& strPublicKey, const std::string& strData ,int length = ENCODE_DATA_LENGTH);

        std::string DecodeRSAData( const std::string& strPrivateKey, const std::string& strData ,int length = DECODE_DATA_LENGTH);

        std::string EncodeRSAKeyFile( const std::string& strPublicKey, const char* strData ,int len);

        std::string DecodeRSAKeyFile( const std::string& strPrivateKey, const char* strData ,int len);

        std::string EncodeAES( const std::string& password, const std::string& data , bool bIsComplement = true);

        std::string DecodeAES( const std::string& password, const std::string& strData , bool bIsComplement = true);

        //cIV  default   unsigned char iv[AES_BLOCK_SIZE]; memset(iv, 0x00, AES_BLOCK_SIZE);

        std::string EncodeCBCAES( const std::string& strKey, const std::string& strdata,unsigned char* cIV);

        std::string DecodeCBCAES( const std::string& strKey, const std::string& strData,unsigned char* cIV);

    }
    
}