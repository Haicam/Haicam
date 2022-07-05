#include "haicam/H264SensorSimulator.hpp"

using namespace haicam;

H264SensorSimulator::H264SensorSimulator() : ImageSensor()
{
}

void H264SensorSimulator::run()
{
    struct __attribute__((__packed__))
    {
        long long millTimestamp;
        bool isKeyframe;
        int length;
    } frameHeader;

    long long lastFrameTime = -1;
    size_t bytesRead = 0, Total_Size = 0;
    const size_t headerFrame = 512; // Taken as a reference only

    FILE *fp = NULL;

    fp = fopen("h264.data", "rb");
    printf("fp=%p\n", fp);

    if (ferror(fp))
    {
        printf("\n Error writing in file");
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    Total_Size = ftell(fp);
    fseek(fp, 1, SEEK_SET);

    while (!feof(fp) && !ferror(fp))
    {

        while (isRunning)
        {
            // fread((void*) &frameHeader, sizeof(frameHeader), 1, fp);

            //  fake data
            frameHeader.length = 1024;
            frameHeader.millTimestamp = 1000;
            lastFrameTime = 0;
            fseek(fp, bytesRead, SEEK_SET);
            
            printf("size of file=%ld\n", Total_Size);
            printf("frameHeader.length=%d\n", frameHeader.length);

            if (lastFrameTime > -1 && frameHeader.millTimestamp > lastFrameTime)
            {
                uv_sleep(frameHeader.millTimestamp - lastFrameTime);
            }

            lastFrameTime = frameHeader.millTimestamp;

            ByteBufferPtr frame = ByteBuffer::create(frameHeader.length);
            //fseek(fp, frameHeader.length, SEEK_SET);
            printf("ftell=%ld\n",ftell(fp));

            size_t result = fread((void *)frame->getDataPtr(), 1, frameHeader.length, fp);
            bytesRead += result;
            printf("No.of bytes Read=%lu\n", bytesRead);
            notify(frame, frameHeader.isKeyframe);
            //fseek(fp, bytesRead, SEEK_SET);
        }
    }
}
H264SensorSimulator::~H264SensorSimulator()
{
}
