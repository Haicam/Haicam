#pragma once
#include <string>
#include <stdio.h>

#define HAICAM_DEV 1

#define H_ASSERT(expr)                                         \
    do                                                         \
    {                                                          \
        if (!(expr))                                           \
        {                                                      \
            fprintf(stderr,                                    \
                    "Assertion failed in %s on line %d: %s\n", \
                    __FILE__,                                  \
                    __LINE__,                                  \
                    #expr);                                    \
            abort();                                           \
        }                                                      \
    } while (0)

#define H_ASSERT_ERR_STR(errStr)                     \
    do                                                         \
    {                                                          \
        if (true)                                           \
        {                                                      \
            fprintf(stderr,                                    \
                    "Assertion failed in %s on line %d: %s\n", \
                    __FILE__,                                  \
                    __LINE__,                                  \
                    errStr);                                    \
            abort();                                           \
        }                                                      \
    } while (0)

#define H_ASSERT_WARN_STR(warnStr)                     \
    do                                                         \
    {                                                          \
        if (true)                                           \
        {                                                      \
            fprintf(stderr,                                    \
                    "Assertion failed in %s on line %d: %s\n", \
                    __FILE__,                                  \
                    __LINE__,                                  \
                    warnStr);                                    \
        }                                                      \
    } while (0)

namespace haicam
{
    class Config
    {
    public:
        static std::string userWritablePath;
    };
}