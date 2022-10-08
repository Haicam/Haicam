#pragma once

#define SIZE_1K             (1  << 10)
#define SIZE_16K            (16 << 10)
#define SIZE_32K            (32 << 10)
#define SIZE_64K            (64 << 10)
#define SIZE_1M             (1  << 20)
#define SIZE_2M             (2  << 20)
#define SIZE_4M             (4  << 20)
#define MAX_ENCODE_DATALENGTH SIZE_64K

extern int CInitEncodeCBCAES();
extern int CEncodeCBCAES( const char* strKey,int KeyLength, const char* indata,int indataLength, char* outdata,int* outdataLength,unsigned char* cIV);
