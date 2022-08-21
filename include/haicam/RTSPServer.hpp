#pragma once
#include "haicam/ImageSensor.hpp"

namespace haicam
{
    class RTSPServer : public ImageObserver
    {
    private:
        RTSPServer();
        
    public:
        static std::shared_ptr<RTSPServer> create()
        {
            return std::shared_ptr<RTSPServer>(new RTSPServer);
        };
        
        ~RTSPServer();
        void onImage(ByteBufferPtr data, bool isKeyFrame);

        int frameReceived;
    };
}
