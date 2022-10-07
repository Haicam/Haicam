#include <stdio.h>
#include <iostream>
#include "haicam/Context.hpp"
#include "haicam/platform/model/Camera.hpp"
#include "haicam/platform/model/Config.hpp"
#include "haicam/platform/model/Led.hpp"
#include "haicam/UserDefault.hpp"
#include "haicam/Utils.hpp"

using namespace haicam;

int main()
{

   Context *context = Context::getInstance();

   Led *led = new platform::model::Led(LED_POWER);

   Config* config = new platform::model::Config();
   config->init();

   Camera *camera = new platform::model::Camera();
   camera->init(context);

   if (Config::getInstance()->isDevelopment()) {
      camera->telnetOn();
   }

   camera->registerSignal();

   if (UserDefault::getInstance()->getBoolForKey("update_firmware")) {
   }

   camera->start();

   context->run();

   delete context;
   delete led;
   delete config;
   delete camera;
   delete UserDefault::getInstance();

   return 0;
}