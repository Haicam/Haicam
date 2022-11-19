/* Code generated by cmd/cgo; DO NOT EDIT. */

/* package command-line-arguments */


#line 1 "cgo-builtin-export-prolog"

#include <stddef.h> /* for ptrdiff_t below */

#ifndef GO_CGO_EXPORT_PROLOGUE_H
#define GO_CGO_EXPORT_PROLOGUE_H

#ifndef GO_CGO_GOSTRING_TYPEDEF
typedef struct { const char *p; ptrdiff_t n; } _GoString_;
#endif

#endif

/* Start of preamble from import "C" comments.  */


#line 3 "discovery.go"





#include "P2Peer.h"
#include "discovery.h"
#include "ifaddrs.h"

#line 1 "cgo-generated-wrapper"

#line 5 "p2p.go"







#include "P2Peer.h"

#line 1 "cgo-generated-wrapper"

#line 3 "serialport.go"





#include "discovery.h"

#line 1 "cgo-generated-wrapper"

#line 3 "websocket.go"





#include "websocket.h"

#line 1 "cgo-generated-wrapper"


/* End of preamble from import "C" comments.  */


/* Start of boilerplate cgo prologue.  */
#line 1 "cgo-gcc-export-header-prolog"

#ifndef GO_CGO_PROLOGUE_H
#define GO_CGO_PROLOGUE_H

typedef signed char GoInt8;
typedef unsigned char GoUint8;
typedef short GoInt16;
typedef unsigned short GoUint16;
typedef int GoInt32;
typedef unsigned int GoUint32;
typedef long long GoInt64;
typedef unsigned long long GoUint64;
typedef GoInt64 GoInt;
typedef GoUint64 GoUint;
typedef __SIZE_TYPE__ GoUintptr;
typedef float GoFloat32;
typedef double GoFloat64;
typedef float _Complex GoComplex64;
typedef double _Complex GoComplex128;

/*
  static assertion to make sure the file is being used on architecture
  at least with matching size of GoInt.
*/
typedef char _check_for_64_bit_pointer_matching_GoInt[sizeof(void*)==64/8 ? 1:-1];

#ifndef GO_CGO_GOSTRING_TYPEDEF
typedef _GoString_ GoString;
#endif
typedef void *GoMap;
typedef void *GoChan;
typedef struct { void *t; void *v; } GoInterface;
typedef struct { void *data; GoInt len; GoInt cap; } GoSlice;

#endif

/* End of boilerplate cgo prologue.  */

#ifdef __cplusplus
extern "C" {
#endif


extern char* GenerateUUID();

extern void GetCameraStreams(char* p0, char* p1, int p2, char* p3, char* p4);

extern void StopPullEvents(char* p0);

extern char* StartPullEvents(int p0, char* p1, char* p2, int p3, char* p4, char* p5);

extern int SetIPAddress(char* p0, int p1, char* p2, char* p3, char* p4, char* p5, int p6);

extern void DiscoveryRun(void* p0);

extern void SetRemoteDescription(long long unsigned int p0, char* p1);

extern void AddICECandidate(long long unsigned int p0, char* p1);

extern void SendData(long long unsigned int p0, void* p1, int p2);

extern void SendVideoSample(long long unsigned int p0, void* p1, int p2, int p3, int p4);

extern void SendAudioSample(long long unsigned int p0, void* p1, int p2, int p3);

extern void ClosePeer(long long unsigned int p0);

extern long long unsigned int CreateOfferer(void* p0, char* p1);

extern long long unsigned int CreateAnswerer(void* p0, char* p1, char* p2);

extern char* GetZWavePortsList();

extern int OpenZWavePort(char* p0);

extern int ZWaveWriteData(void* p0, int p1);

extern int ZWaveReadData(void** p0);

extern void WebSocketStart(char* p0, void* p1);

extern void SendWebSocketData(long long unsigned int p0, void* p1, int p2);

extern char* GetExePath();

extern char* DirectoryUsage(char* p0);

#ifdef __cplusplus
}
#endif