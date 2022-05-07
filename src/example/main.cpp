#include <stdio.h>
#include "haicam/Context.hpp"

using namespace haicam;

int main() {

   printf("Hello,Haicam!\n");

   Context* context = Context::getInstance();
   context->run();

   return 0;
}