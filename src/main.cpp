#include <stdio.h>
#include "HaicamContext.hpp"
int main() {

   printf("Hello, Haicam!\n");

   HaicamContext* haicamContext = HaicamContext::getInstance();
   haicamContext->run();

   return 0;
}