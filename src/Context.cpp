#include "haicam/Context.hpp"
#include "haicam/RSAKey.hpp"

using namespace haicam;

Context* Context::instance = NULL;
std::mutex Context::mtx;

Context::Context() : uv_loop(NULL), localAddr(0)
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

std::string getServerRSAKey1024()
{
    H_ASSERT_ERR_STR("Does not support getServerRSAKey1024 func");
    return "";
}

std::string getAESKey1024()
{
    H_ASSERT(false);
    return "";
}

std::string getServerRSAKey2048()
{
#if HAICAM_DEV
    return SERVER_DEV_RSA2048_PUBLIC_KEY;
#else
    return SERVER_RSA2048_PUBLIC_KEY;
#endif
}

std::string getAESKey2048()
{
    return "";
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