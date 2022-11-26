#include "haicam/ServerConnection.hpp"

using namespace haicam;

ServerConnection *ServerConnection::instance = NULL;

ServerConnection::ServerConnection(Context *context, std::string serverIp, int serverPort, std::string serverPublicKey) : SSLTCPClient(context, serverIp, serverPort, serverPublicKey)
{
}

ServerConnection::~ServerConnection()
{
}

ServerConnection* ServerConnection::createInstance(Context *context, std::string serverIp, int serverPort, std::string serverPublicKey)
{
    H_ASSERT(instance == NULL);

    if (instance == NULL)
    {
        instance = new ServerConnection(context, serverIp, serverPort, serverPublicKey);
    }

    return instance;
}

ServerConnection* ServerConnection::getInstance()
{
    H_ASSERT(instance != NULL);

    return instance;
}