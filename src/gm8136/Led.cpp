#include "haicam/gm8136/Led.hpp"

using namespace haicam::gm8136;

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
