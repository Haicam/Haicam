#include <stdio.h>
#include <iostream>
#include "haicam/Context.hpp"
#include "haicam/platform/model/Camera.hpp"
#include "haicam/platform/model/Config.hpp"
#include "haicam/platform/model/Watchdog.hpp"
#include "haicam/UserDefault.hpp"
#include "haicam/Utils.hpp"

using namespace haicam;

void bye(void)
{
   printf("That was all, folks\n");
}

int main()
{
   /*
   The atexit() function registers the given function to be called
   at normal process termination, either via exit(3) or via return
   from the program's main().  Functions so registered are called in
   the reverse order of their registration;
   */
   atexit(bye);

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