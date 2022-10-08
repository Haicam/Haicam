#include "haicam/Watchdog.hpp"
#include "haicam/Config.hpp"

using namespace haicam;

Watchdog::Watchdog(Context *context) : context(context), timer()
{
}

Watchdog::~Watchdog()
{
    this->stop();
}

void Watchdog::start()
{
    if(timer.get() != NULL) return;

    timer = Timer::create(context, Config::getInstance()->getWatchdogTick(),  Config::getInstance()->getWatchdogTick()); // every 3 seconds
    timer->onTimeoutCallback = std::bind(&Watchdog::update, this);

    this->enable();
}

void Watchdog::stop()
{
    if(timer.get() != NULL) 
    {
        this->disable();
        timer->stop();
        timer.reset();
    }
}

void Watchdog::enable()
{
    //overwrite by child class
}

void Watchdog::disable()
{
    //overwrite by child class
}

void Watchdog::update()
{
    //overwrite by child class
}
