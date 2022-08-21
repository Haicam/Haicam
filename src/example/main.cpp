#include <stdio.h>
#include "haicam/Context.hpp"
#include "haicam/Camera.hpp"

using namespace haicam;

int main() {

   Context* context = Context::getInstance();
   
   Camera* camera = Camera::getInstance();
   camera->init(context);

   camera->start();

   context->run();

   return 0;
}