#include "haicam/Context.hpp"

using namespace haicam;

Context *Context::instance = NULL;

Context::Context() : uv_loop(NULL)
{
    uv_loop = uv_default_loop();
}

Context *Context::getInstance()
{
    if (instance == NULL)
    {
        instance = new Context();
    }

    return instance;
}

int Context::run()
{
    // blocked until running finished
    int ret = uv_run(uv_loop, UV_RUN_DEFAULT);
    uv_loop = NULL;

    return ret;
}

void Context::stop()
{
    uv_stop(uv_loop);
}

Context::~Context()
{
}