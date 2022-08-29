#pragma once
#include "haicam/SSLTCPClient.hpp"

namespace haicam {
    class ServerConnection: public SSLTCPClient
    {
    private:
    public:
        ServerConnection(Context *context, std::string serverIp, int serverPort, std::string serverPublicKey);
        ~ServerConnection();
    };
}