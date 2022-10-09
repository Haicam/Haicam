#include <stdio.h>
#include <iostream>
#include "haicam/Context.hpp"
#include "haicam/platform/model/Camera.hpp"
#include "haicam/platform/model/Config.hpp"
#include "haicam/platform/model/Watchdog.hpp"
#include "haicam/UserDefault.hpp"
#include "haicam/Utils.hpp"

using namespace haicam;

int main()
{

   Context *context = Context::getInstance();

   Config* config = new platform::model::Config();
   config->init();

   Camera *camera = new platform::model::Camera();
   camera->init(context);

   camera->start();

   context->run();

   delete context;
   delete config;
   delete camera;
   delete UserDefault::getInstance();

   return 0;
}