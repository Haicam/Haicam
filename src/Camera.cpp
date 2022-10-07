#include "haicam/Camera.hpp"
#include "haicam/Config.hpp"
#include "haicam/Utils.hpp"
#include <signal.h>

using namespace haicam;

Camera* Camera::instance = NULL;

Camera::Camera()
{
}

Camera::~Camera()
{
    instance = NULL;
}

void Camera::init(Context* context)
{
    if(this->context != NULL) return;
    
    this->context = context;

    instance = this;
}

Camera* Camera::getInstance()
{
    H_ASSERT(instance != NULL);
    return instance;
}

void Camera::start()
{
    H_ASSERT(context != NULL);
}

void Camera::stop()
{

}

void Camera::telnetOn()
{
    Utils::log("Do not support Camera::telnetOn");
}

void Camera::telnetOff()
{
     Utils::log("Do not support Camera::telnetOfflnetOff");
}

void Camera::processSignal(int sig)
{
    if ((SIGTERM == sig) || (SIGINT == sig) || (SIGSEGV == sig)) {
        exit(EXIT_FAILURE);
    }
}

void Camera::registerSignal()
{
    signal(SIGSEGV, Camera::processSignal);
    signal(SIGINT, Camera::processSignal);
    signal(SIGTERM, Camera::processSignal);
    signal(SIGCHLD, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGILL, SIG_IGN);
}

void Camera::upgradeFirmware()
{
    Utils::log("Do not support Camera::upgradeFirmware");
}