#include "haicam/Utils.hpp"
#include "haicam/Config.hpp"
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/stat.h> //S_IRWXU
#include <unistd.h>   //access
#include <stdio.h>
#include <pthread.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <sstream>
#ifdef _WIN32
#define localtime_r(T, Tm) (localtime_s(Tm, T) ? NULL : Tm)
#endif

#ifdef __ANDROID__
#include <android/log.h>
#define printf(...) __android_log_print(ANDROID_LOG_DEBUG, "haicam_log", __VA_ARGS__);
#endif

#include <openssl/rand.h>

using namespace haicam;

std::string Utils::getFastString(Json::Value value)
{
    Json::FastWriter writer;
    return writer.write(value);
}

Json::Value Utils::getJsonFromString(std::string str)
{
    Json::Reader reader;
    Json::Value value;
    if (reader.parse(str, value))
    {
        return value;
    }
    else
    {
        value.clear();
        return value;
    }
}

std::string Utils::uint32ToNetworkString(uint32_t value)
{
    std::string ret = "";
    value = htonl(value);
    ret.append(1, (value & 0xFF));
    ret.append(1, ((value >> 8) & 0xFF));
    ret.append(1, ((value >> 16) & 0xFF));
    ret.append(1, ((value >> 24) & 0xFF));
    return ret;
}

uint32_t Utils::networkStringToUint32(std::string buf, size_t offset)
{
    std::string strbuf = buf.substr(offset, 4);
    uint32_t ret = *(uint32_t *)strbuf.data();
    ret = ntohl(ret);
    return ret;
}

uint16_t Utils::networkStringToUint16(std::string buf, size_t offset)
{
    std::string strbuf = buf.substr(offset, 2);
    uint16_t ret = *(uint16_t *)strbuf.data();
    ret = ntohs(ret);
    return ret;
}

bool Utils::checkIsFileExsit(std::string fullPath)
{
    if (access(fullPath.c_str(), F_OK) == -1)
    {
        return false;
    }
    return true;
}

void Utils::makeDir(std::string dir)
{
    if (!checkIsFileExsit(dir))
    {
#ifdef _WIN32
        mkdir(dir.c_str());
#else
        mkdir(dir.c_str(), S_IRWXU);
#endif
    }
}

static void _log(const char *format, va_list args)
{
    int bufferSize = 1024 * 32;
    char *buf = NULL;

    do
    {
        buf = new (std::nothrow) char[bufferSize];
        if (buf == NULL)
            return; // not enough memory

        int ret = vsnprintf(buf, bufferSize - 3, format, args);
        if (ret < 0)
        {
            bufferSize *= 2;

            delete[] buf;
        }
        else
            break;

    } while (true);

    strcat(buf, "\n");
    char dtime[100];
    struct timeval curTime;
    gettimeofday(&curTime, NULL);
    time_t now = time(0);
    int milli = curTime.tv_usec / 1000;
    struct tm timeInfo;
    localtime_r(&now, &timeInfo);
    strftime(dtime, 100, "%Y-%m-%d %H:%M:%S", &timeInfo);
    // printf("%s.%d:[%zu] %s",dtime,milli, pthread_self(), buf);

    char out[500]; // < android logcat max line length

#ifdef __ANDROID__
    size_t len = strlen(buf);
    bool firstTime = true;
    for (int i = 0; i < len;)
    {
        memset(out, 0, 500);
        if (len - i < 500)
        {
            memcpy(out, buf + i, len - i);
            i += len - i;
        }
        else
        {
            memcpy(out, buf + i, 499);
            i += 499;
        }

        if (firstTime)
        {
            printf("[%zu] %s", pthread_self(), out);
            firstTime = false;
        }
        else
        {
            printf("<<%s", out);
        }
    }
#else
    printf("%s.%d:[%zu] %s", dtime, milli, pthread_self(), buf);
#endif

    delete[] buf;
}

void Utils::log_va(const char *format, va_list args)
{
    if (!Config::getInstance()->isDevelopment()) 
    {   
        return;
    }
    _log(format, args);
}

void Utils::log(const char *format, ...)
{
    if (!Config::getInstance()->isDevelopment()) 
    {   
        return;
    }
    va_list args;
    va_start(args, format);
    _log(format, args);
    va_end(args);
}

/*
* Credit: https://gist.github.com/kvelakur/9069c9896577c3040030
* Generate a Version 4 UUID according to RFC-4122
*/
std::string Utils::uuidV4()
{
    char uuidv4[36+1];

    union
    {
        struct
        {
            uint32_t time_low;
            uint16_t time_mid;
            uint16_t time_hi_and_version;
            uint8_t clk_seq_hi_res;
            uint8_t clk_seq_low;
            uint8_t node[6];
        };
        uint8_t __rnd[16];
    } uuid;

    int rc = RAND_bytes(uuid.__rnd, sizeof(uuid));

    H_ASSERT(rc == 1);

    // Refer Section 4.2 of RFC-4122
    // https://tools.ietf.org/html/rfc4122#section-4.2
    uuid.clk_seq_hi_res = (uint8_t)((uuid.clk_seq_hi_res & 0x3F) | 0x80);
    uuid.time_hi_and_version = (uint16_t)((uuid.time_hi_and_version & 0x0FFF) | 0x4000);

    snprintf(uuidv4, sizeof(uuidv4), "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
             uuid.time_low, uuid.time_mid, uuid.time_hi_and_version,
             uuid.clk_seq_hi_res, uuid.clk_seq_low,
             uuid.node[0], uuid.node[1], uuid.node[2],
             uuid.node[3], uuid.node[4], uuid.node[5]);

    return (const char*)uuidv4;
}

std::string Utils::executeSystemCommand(std::string command)
{
    std::string strData = "";
    FILE * fp = NULL;
    char buffer[128];
    fp=popen(command.c_str(),"r");
    if (fp) {
        while(fgets(buffer,sizeof(buffer),fp)){
            strData.append(buffer);
        }
        pclose(fp);
    }
    return strData;
}