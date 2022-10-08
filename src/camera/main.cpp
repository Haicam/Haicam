#include <stdio.h>
#include <iostream>
#include "haicam/Context.hpp"
#include "haicam/platform/model/Camera.hpp"
#include "haicam/platform/model/Config.hpp"
#include "haicam/platform/model/Led.hpp"
#include "haicam/platform/model/Watchdog.hpp"
#include "haicam/UserDefault.hpp"
#include "haicam/Utils.hpp"

using namespace haicam;

int main()
{

   Context *context = Context::getInstance();

   Led *led = new platform::model::Led(LED_POWER);

   Config* config = new platform::model::Config();
   config->init();
   
   Watchdog* watchdog = new platform::model::Watchdog(context);

   Camera *camera = new platform::model::Camera();
   camera->init(context);

   if (Config::getInstance()->isDevelopment()) {
      camera->telnetOn();
   } else {
      camera->startWatchdog();
      watchdog->start();
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
   delete watchdog;

   return 0;
}