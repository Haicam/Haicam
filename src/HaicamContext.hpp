#ifndef HAICAMCONTEXT_HPP
#define HAICAMCONTEXT_HPP

extern "C" {
     #include <uv.h>
}

class HaicamContext
{
   public:
       static HaicamContext* getInstance();
       ~HaicamContext();

       int run();
       void stop();

       uv_loop_t *uv_loop;
   private:
       HaicamContext();
       static HaicamContext* instance;
};

#endif