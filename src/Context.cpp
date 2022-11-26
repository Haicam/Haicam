#include "haicam/Context.hpp"
#include "haicam/RSAKey.hpp"
#include "haicam/UserDefault.hpp"

using namespace haicam;

#define MAKE_VALGRIND_HAPPY()                       \
  do {                                              \
    close_loop(uv_default_loop());                  \
    uv_loop_close(uv_default_loop());  \
  } while (0)

Context *Context::instance = NULL;
std::mutex Context::mtx;

Context::Context() : uv_loop(NULL)
{
    setenv("UV_THREADPOOL_SIZE",Config::getInstance()->getThreadPoolSize(),1); // for max uv_queue_work
    uv_loop = (uv_loop_t *)malloc(sizeof(uv_loop_t));
    uv_loop_init(uv_loop);
}

Context *Context::getInstance()
{
    if (instance == NULL)
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (instance == NULL)
        {
            instance = new Context();
        }
    }

    return instance;
}

int Context::run()
{
    // blocked until running finished
    int ret = uv_run(uv_loop, UV_RUN_DEFAULT);

    return ret;
}

void Context::stop()
{
    uv_stop(uv_loop);
}

uint64_t Context::getCurrentMillSecs()
{
    return uv_now(uv_loop);
}

/* Fully close a loop */
static void close_walk_cb(uv_handle_t* handle, void* arg) {
  if (!uv_is_closing(handle))
    uv_close(handle, NULL);
}

static void close_loop(uv_loop_t* loop) {
  uv_walk(loop, close_walk_cb, NULL);
  uv_run(loop, UV_RUN_DEFAULT);
}

Context::~Context()
{
    if (uv_loop != NULL)
    {    
        if (!uv_is_closing((uv_handle_t *)uv_loop))
        {
            //close_loop(uv_loop); // MAKE VALGRIND HAPPY
            uv_loop_close(uv_loop);
        }
        free(uv_loop);
        uv_loop = NULL;
    }
    instance = NULL;

    //MAKE_VALGRIND_HAPPY();
}