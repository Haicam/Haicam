#include "HaicamContext.hpp"

HaicamContext* HaicamContext::instance = NULL;

HaicamContext::HaicamContext() {
    uv_loop = uv_default_loop();
}

HaicamContext* HaicamContext::getInstance() {

    if (instance == NULL) {
        instance = new HaicamContext();
    }

    return instance;
}

int HaicamContext::run() {
    // blocked until running finished
    int ret = uv_run(uv_loop, UV_RUN_DEFAULT);
    uv_loop = NULL;
    
    return ret;
}

void HaicamContext::stop() {
    uv_stop(uv_loop);
}

HaicamContext::~HaicamContext() {

}