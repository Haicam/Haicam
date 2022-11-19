#ifndef IFADDRS_H_included
#define IFADDRS_H_included

#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

const char* getInterfacesAndIPs();

#ifdef __cplusplus
}
#endif

#endif