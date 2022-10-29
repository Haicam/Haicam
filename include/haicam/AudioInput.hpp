#pragma once

namespace haicam
{

    class AudioInput
    {
    private:
    public:
        AudioInput(){};
        virtual ~AudioInput(){};
        virtual void open(){};
        virtual void onData(void *data, int len){};
        virtual void close(){};
    };

}
