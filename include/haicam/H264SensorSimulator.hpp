#pragma once
#include "haicam/ImageSensor.hpp"
#include "haicam/Runnable.hpp"
#include <iostream>
#include <fstream>
#include <list>
#include <memory>

namespace haicam
{

    class H264SensorSimulator : public ImageSensor  //class H264 derived from class Imagesensor
    {
    private:
        H264SensorSimulator();  //constructor

    public:
        ~H264SensorSimulator(); //destructor

        void run();
    };

    H264SensorSimulator::H264SensorSimulator() : ImageSensor() // Aceesing class ouside fun using scope resolution 
    {
    }

    std::shared_ptr<H264SensorSimulator> create()  //polymorphic function wrap
    {
        return std::shared_ptr<H264SensorSimulator>(new H264SensorSimulator);
    }

    H264SensorSimulator::~H264SensorSimulator() //destuctor function
    {
    }

    void H264SensorSimulator::run() //Reading Raw data from simulator.data 
    {

       FILE *fp = fopen("h264.data","rb");

         if(fp)
             {
              fseek (fp, 0, SEEK_END);
              int size = ftell(fp);
              fseek (fp, 1, SEEK_SET);

              char * buffer = new char [size];
              std::cout << "Reading " << size << " characters... ";
              fread (buffer,size,1,fp);

              delete[] buffer;
              fclose(fp);
             }

             else
                 {
                  std::cout << "error:The file can not be found";
                  fclose(fp);
                 }
}
