#pragma once
#include <stdlib.h>
#include <memory>
#include <functional>
#include <stdarg.h>

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

#define H_FUNC(name, params_and_body) \
        typedef struct \
        { \
            static void func \
            params_and_body \
        } name

#define H_TFUNC(type, name, params_and_body) \
        typedef struct \
        { \
            static type func \
            params_and_body \
        } name
        
#define H_BIND(name) \
        std::bind(name::func)
        
#define H_BINDV(name, ...) \
        std::bind(name::func, __VA_ARGS__)


#define H_CFG_VAR(type, name, value) inline virtual type name () { return value; }
// ex: H_NEW_SP(std::string, str, ("hello"));
#define H_NEW_SP(type, name, init) std::shared_ptr< type > name (new type init)
#define H_NEWN_SP(type, name, NS_init) std::shared_ptr< type > name (new NS_init)
// H_DEF_SP(std:string, str)
#define H_DEF_SP(type, name) std::shared_ptr< type > name 
// H_MK_SP(std:string, ("hello"))
#define H_MK_SP(type, init) std::make_shared<type> init

#define H_MEM_SP(type, name, size) std::shared_ptr< type > name (new type [ size ], std::default_delete< type []>())
#define H_MK_MEM_SP(type, size) std::shared_ptr< type > (new type [ size ], std::default_delete< type []>())
