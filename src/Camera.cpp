#include "haicam/Camera.hpp"
#include "haicam/RSAKey.hpp"

using namespace haicam;


Camera* Camera::instance = NULL;
std::mutex Camera::mtx;

void Camera::init(Context* context)
{
    if(this->context != NULL) return;

    this->context = context;
}

Camera::Camera():context(NULL)
{
}

Camera::~Camera()
{
}

Camera* Camera::getInstance()
{
    if (instance == NULL)
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (instance == NULL)
        {
            instance = new Camera();
        }
    }

    return instance;
}

void Camera::start()
{
    H_ASSERT(context != NULL);
}

void Camera::stop()
{

}