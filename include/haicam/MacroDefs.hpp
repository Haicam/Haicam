#pragma once
#include <stdlib.h>
#include <memory>

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

#define H_ASSERT_NOT(expr)                                         \
    do                                                         \
    {                                                          \
        if (expr)                                           \
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

#define H_CFG_VAR(type, name, value) inline virtual type name () { return value; }
// ex: H_OBJ_PTR(std::string, str, ("hello"));
#define H_OBJ_PTR(type, name, init) std::shared_ptr< type > name (new type init)
// H_TP_PTR(std:string, str)
#define H_TP_PTR(type, name) std::shared_ptr< type > name 
// H_MK_PTR(std:string, str)
#define H_MK_PTR(type, init) std::make_shared<type> init

#define H_MEM_PTR(type, name, size) std::shared_ptr< type > name (new type [ size ], std::default_delete< type []>())