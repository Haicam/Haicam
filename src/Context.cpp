#include "haicam/Context.hpp"

using namespace haicam;

Context* Context::instance = NULL;
std::mutex Context::mtx;

Context::Context() : uv_loop(NULL)
{
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

Context::~Context()
{
    if (uv_loop != NULL)
    {
        if(!uv_is_closing((uv_handle_t *)uv_loop))
        {
            uv_loop_close(uv_loop);
        }
        free(uv_loop);
        uv_loop = NULL;
    }
    instance = NULL;
}