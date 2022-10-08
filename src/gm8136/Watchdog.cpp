#include "haicam/platform/Watchdog.hpp"
#include "haicam/Utils.hpp"
#include "haicam/Config.hpp"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/watchdog.h>

using namespace haicam::platform;

static int watchdog_fd = 0;

Watchdog::Watchdog(Context *context) : haicam::Watchdog(context)
{
}

void Watchdog::enable()
{
    haicam::Watchdog::enable();

    int interval;
    int bootstatus;

    if (!watchdog_fd)
    {
        interval = Config::getInstance()->getWatchdogTimeout();
        watchdog_fd = open(Config::getInstance()->getWatchdogDevice().c_str(), O_RDWR);
        ioctl(watchdog_fd, WDIOC_SETTIMEOUT, &interval);
        ioctl(watchdog_fd, WDIOC_GETTIMEOUT, &interval);
        Utils::log("Watchdog interval set to %d\n", interval);
    }
}

void Watchdog::update()
{
    haicam::Watchdog::update();

    if (watchdog_fd)
    {
        ioctl(watchdog_fd, WDIOC_KEEPALIVE, NULL);
    }
}

void Watchdog::disable()
{
    haicam::Watchdog::disable();

    if (watchdog_fd)
    {
        write(watchdog_fd, "V", 1);
        close(watchdog_fd);
        watchdog_fd = 0;
    }
}
