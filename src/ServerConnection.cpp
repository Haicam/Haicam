#include "haicam/ServerConnection.hpp"

using namespace haicam;

ServerConnection::ServerConnection(Context *context, std::string serverIp, int serverPort, std::string serverPublicKey)
:SSLTCPClient(context, serverIp, serverPort, serverPublicKey)
{

}

ServerConnection::~ServerConnection()
{

}