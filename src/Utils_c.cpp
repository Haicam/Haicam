#include "haicam/Utils.hpp"
#include <stdarg.h>

extern "C" {
    #include "haicam/Utils.h"
}

using namespace haicam;

void haicam_utils_log(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    Utils::log_va(format, args);
    va_end(args);
}