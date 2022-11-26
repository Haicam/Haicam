#pragma once
#include "haicam/SSLTCPClient.hpp"

namespace haicam {
    class ServerConnection: public SSLTCPClient
    {
    private:
        static ServerConnection* instance;
        
        ServerConnection(Context *context, std::string serverIp, int serverPort, std::string serverPublicKey);
    public:
        static ServerConnection* createInstance(Context *context, std::string serverIp, int serverPort, std::string serverPublicKey);
        static ServerConnection* getInstance();
        ~ServerConnection();
    };
}