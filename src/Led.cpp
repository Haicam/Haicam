#include "haicam/Led.hpp"

using namespace haicam;

Led::Led(LedType type)
{
    this->type = type;
}

Led::~Led(){

}

void Led::on()
{
    this->status = LED_ON;
}

void Led::off()
{
    this->status = LED_OFF;
}

void Led::setStatus(haicam::LedStatus status)
{
    this->status = status;
}

haicam::LedStatus Led::getStatus()
{
    return this->status;
}
