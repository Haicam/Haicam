#include "haicam/platform/Camera.hpp"
#include "haicam/Utils.hpp"

using namespace haicam::platform;

Camera::Camera()
{
}

Camera::~Camera()
{
}

void Camera::init(Context* context)
{
    haicam::Camera::init(context);
}

void Camera::telnetOn()
{
    FILE* fp;
	fp = popen("/usr/sbin/telnetd &", "r");
	pclose(fp);
    Utils::log("Haicam telnet on");
}

void Camera::telnetOff()
{
    FILE* fp;
	fp = popen("killall telnetd", "r");
	pclose(fp);
    Utils::log("Haicam telnet off");
}

void Camera::upgradeFirmware()
{
    
}