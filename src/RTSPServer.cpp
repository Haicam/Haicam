#include "haicam/RTSPServer.hpp"

using namespace haicam;

RTSPServer::RTSPServer(): frameReceived(0) {

}

RTSPServer::~RTSPServer() {

}

void RTSPServer::onImage(ByteBufferPtr data, bool isKeyFrame) {
    frameReceived ++;

}