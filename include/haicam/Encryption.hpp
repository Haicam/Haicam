#pragma once
#include <string>

namespace haicam{

    namespace Encryption
    {

        bool generateKey(std::string PriKeyPath, std::string PubKeyPath, int keyBits);

        std::string EncodeRSAData( const std::string& strPublicKey, const std::string& strData, int keyBytes);

        std::string DecodeRSAData( const std::string& strPrivateKey, const std::string& strData, int keyBytes);

        std::string EncodeRSABlock( const std::string& strPublicKey, const char* strData, int len);

        std::string DecodeRSABlock( const std::string& strPrivateKey, const char* strData, int len);

        std::string EncodeAES( const std::string& strKey, const std::string& data , bool bIsComplement = true);

        std::string DecodeAES( const std::string& strKey, const std::string& strData , bool bIsComplement = true);

        std::string EncodeCBCAES( const std::string& strKey, const std::string& strdata,unsigned char* cIV);

        std::string DecodeCBCAES( const std::string& strKey, const std::string& strData,unsigned char* cIV);

    }
    
}