#include "haicam/GatewayConnection.hpp"

using namespace haicam;

GatewayConnection::GatewayConnection(Context *context, std::string serverIp, int serverPort, std::string serverPublicKey)
:SSLTCPClient(context, serverIp, serverPort, serverPublicKey)
{

}

GatewayConnection::~GatewayConnection()
{

}