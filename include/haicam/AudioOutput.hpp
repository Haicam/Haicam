#pragma once

namespace haicam
{

    class AudioOutput
    {
    private:
    public:
        AudioOutput(){};
        virtual ~AudioOutput(){};
        virtual void open(){};
        virtual void onData(void* data, int len){};
        virtual void close(){};
    };


}
