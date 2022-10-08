#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <openssl/aes.h>
#include "HardwareEncryption.h"
#include "haicam/Config.h"

/**
 * @brief security algorithm argument and address
 */
typedef struct esreq_tag {
    int          algorithm;     ///< security algorithm, such as Algorithm_DES/Algorithm_Triple_DES/Algorithm_AES_128/Algorithm_AES_192/Algorithm_AES_256
    int          mode;          ///< security mode, such as ECB_mode/CBC_mode/CTR_mode/CFB_mode/OFB_mode
    
    unsigned int data_length;   ///< size of data, max value = 4096, include 16 bytes IV
    unsigned int key_length;    ///< size of key
    unsigned int IV_length;     ///< size of Initial Vector
    
    unsigned int key_addr[8];   ///< key array, it maybe be modify by security algorithm
    unsigned int IV_addr[4];    ///< Initial Vector array, it maybe be modify by security algorithm
    
    unsigned int DataIn_addr;   ///< input data address point
    unsigned int DataOut_addr;  ///< output data address point
    
    unsigned int Key_backup;    ///< backup key
    unsigned int IV_backup;     ///< backup Initial Vector
} esreq;

/* security mode */
#define ECB_mode			    0x00
#define CBC_mode			    0x10
#define CTR_mode			    0x20
#define CFB_mode			    0x40
#define OFB_mode			    0x50

/* security algorithm */
#define Algorithm_DES			0x0
#define Algorithm_Triple_DES	0x2
#define Algorithm_AES_128		0x8
#define Algorithm_AES_192		0xA
#define Algorithm_AES_256		0xC

/* Use 'e' as magic number */
#define IOC_MAGIC  'e'

/**
 * \b ioctl(security_fd, ES_GETKEY, &tag)
 *
 * \arg get random key and Initial Vector
 * \arg parameter :
 * \n \b \e pointer \b \e tag : argument from user space ioctl parameter, it means structure esreq
 */
#define ES_GETKEY           _IOWR(IOC_MAGIC, 8, esreq)
/**
 * \b ioctl(security_fd, ES_GETKEY, &tag)
 *
 * \arg encrypt data, must set key and Initial Vector first
 * \arg parameter :
 * \n \b \e pointer \b \e tag : argument from user space ioctl parameter, it means structure esreq
 */
#define ES_ENCRYPT          _IOWR(IOC_MAGIC, 9, esreq)
/**
 * \b ioctl(security_fd, ES_GETKEY, &tag)
 *
 * \arg decrypt data, must set key and Initial Vector first
 * \arg parameter :
 * \n \b \e pointer \b \e tag : argument from user space ioctl parameter, it means structure esreq
 */
#define ES_DECRYPT          _IOWR(IOC_MAGIC, 10, esreq)
/**
 * \b ioctl(security_fd, ES_AUTO_ENCRYPT, &tag)
 *
 * \arg auto encrypt data, key and Initial Vector will be auto generated
 * \arg parameter :
 * \n \b \e pointer \b \e tag : argument from user space ioctl parameter, it means structure esreq
 */
#define ES_AUTO_ENCRYPT    	_IOWR(IOC_MAGIC, 11, esreq)
/**
 * \b ioctl(security_fd, ES_AUTO_DECRYPT, &tag)
 *
 * \arg auto decrypt data, key and Initial Vector will be auto generated
 * \arg parameter :
 * \n \b \e pointer \b \e tag : argument from user space ioctl parameter, it means structure esreq
 */
#define ES_AUTO_DECRYPT    	_IOWR(IOC_MAGIC, 12, esreq)

#define SECURITY_ALG        Algorithm_AES_128
#define SECURITY_MODE       CBC_mode
//#define AES_BLOCK_SIZE 16

//unsigned char aes_key[]={0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF};
static int static_handler = -1;
static esreq        wrq;

int CInitEncodeCBCAES(){
    if (static_handler == -1) {
        static_handler = open(haicam_cfgGetSecurityDevice(), O_RDWR);
        memset((void *)&wrq, 0, sizeof(esreq));
        wrq.DataIn_addr  = (unsigned int) mmap(NULL, (MAX_ENCODE_DATALENGTH+16)*2, PROT_READ|PROT_WRITE, MAP_SHARED, static_handler, 0);
        wrq.DataOut_addr = (unsigned int)((char*)wrq.DataIn_addr + MAX_ENCODE_DATALENGTH + 16);
    }
    if (MAP_FAILED == (void*)(wrq.DataIn_addr)) {
        memset((void *)&wrq, 0, sizeof(esreq));
        wrq.DataIn_addr  = (unsigned int) mmap(NULL, (MAX_ENCODE_DATALENGTH+16)*2, PROT_READ|PROT_WRITE, MAP_SHARED, static_handler, 0);
        wrq.DataOut_addr = (unsigned int)((char*)wrq.DataIn_addr + MAX_ENCODE_DATALENGTH + 16);
    }
    if (static_handler <= 0) {
        static_handler = -1;
        usleep(200000);
        return -1;
    }
    return 1;
}

int CEncodeCBCAES( const char* strKey,int KeyLength, const char* indata,int indataLength, char* outdata,int* outdataLength,unsigned char* cIV)
{
    if (static_handler == -1) {
        static_handler = open(haicam_cfgGetSecurityDevice(), O_RDWR);
        memset((void *)&wrq, 0, sizeof(esreq));
        wrq.DataIn_addr  = (unsigned int) mmap(NULL, (MAX_ENCODE_DATALENGTH+16)*2, PROT_READ|PROT_WRITE, MAP_SHARED, static_handler, 0);
        wrq.DataOut_addr = (unsigned int)((char*)wrq.DataIn_addr + MAX_ENCODE_DATALENGTH + 16);
    }
    if (MAP_FAILED == (void*)(wrq.DataIn_addr)) {
        memset((void *)&wrq, 0, sizeof(esreq));
        wrq.DataIn_addr  = (unsigned int) mmap(NULL, (MAX_ENCODE_DATALENGTH+16)*2, PROT_READ|PROT_WRITE, MAP_SHARED, static_handler, 0);
        wrq.DataOut_addr = (unsigned int)((char*)wrq.DataIn_addr + MAX_ENCODE_DATALENGTH + 16);
    }
    if (static_handler <= 0) {
        static_handler = -1;
        usleep(200000);
        handlerCreateFailed();
        return -1;
    }
    if (MAP_FAILED == (void*)(wrq.DataIn_addr)) {
        usleep(100000);
        return -1;
    }
    wrq.key_length = KeyLength;
    wrq.IV_length = AES_BLOCK_SIZE;
    memcpy((char*)wrq.DataIn_addr, indata, indataLength);
    memcpy((char*)wrq.key_addr, strKey, KeyLength);
    wrq.key_addr[0] = htonl(wrq.key_addr[0]);
    wrq.key_addr[1] = htonl(wrq.key_addr[1]);
    wrq.key_addr[2] = htonl(wrq.key_addr[2]);
    wrq.key_addr[3] = htonl(wrq.key_addr[3]);
    memcpy(wrq.IV_addr, cIV, AES_BLOCK_SIZE);
    wrq.IV_addr[0] = htonl(wrq.IV_addr[0]);
    wrq.IV_addr[1] = htonl(wrq.IV_addr[1]);
    wrq.IV_addr[2] = htonl(wrq.IV_addr[2]);
    wrq.IV_addr[3] = htonl(wrq.IV_addr[3]);
    
    wrq.data_length = indataLength+16;
    
    wrq.algorithm = SECURITY_ALG;
    wrq.mode      = SECURITY_MODE;
    int ret = ioctl(static_handler, ES_ENCRYPT, &wrq);
    
    memcpy(outdata, (const void*)wrq.DataOut_addr, indataLength);
    memcpy(cIV, (const void*)(wrq.DataOut_addr+(indataLength-AES_BLOCK_SIZE)), AES_BLOCK_SIZE);
    return ret;
}

