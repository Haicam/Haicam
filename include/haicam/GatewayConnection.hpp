#pragma once
#include "haicam/SSLTCPClient.hpp"

namespace haicam {
    class GatewayConnection : public SSLTCPClient
    {
    private:

    public:
        GatewayConnection(Context *context, std::string serverIp, int serverPort, std::string serverPublicKey);
        ~GatewayConnection();
    };
    
}