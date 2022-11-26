#pragma once
#include <string>
#include "haicam/FrameCommand.hpp"

namespace haicam{

    namespace Encryption
    {

        bool generateRSAKeyPair(std::string& strPublicKey, std::string& strPrivateKey, int keyBits);

        std::string EncodeRSAData( const std::string& strPublicKey, const std::string& strData, int keyBytes, int paddingMode = 0);

        std::string DecodeRSAData( const std::string& strPrivateKey, const std::string& strData, int keyBytes, int paddingMode = 0);

        std::string EncodeRSABlock( const std::string& strPublicKey, const char* strData, int len, int paddingMode = 0);

        std::string DecodeRSABlock( const std::string& strPrivateKey, const char* strData, int len, int paddingMode = 0);

        std::string EncodeAES( const std::string& strKey, const std::string& data , bool bIsComplement = true);

        std::string DecodeAES( const std::string& strKey, const std::string& strData , bool bIsComplement = true);

        std::string EncodeCBCAES( const std::string& strKey, const std::string& strdata,unsigned char* cIV);

        std::string DecodeCBCAES( const std::string& strKey, const std::string& strData,unsigned char* cIV);

        std::string packFrame(uint32 frameNum, uint8 cmd, uint8 cmdType, std::string payload, uint32 remoteAddr, uint8 encryptType, std::string masterKey = "");

        bool unpackFrame(uint8& cmd, uint8& cmdType, uint32& fromAddr, uint32& frameNum, std::string &payload, std::string &lastBuffer, std::string& buffer, std::string masterKey = "");

    }
    
}