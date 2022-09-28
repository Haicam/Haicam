#pragma once
#include <string>
#include <mutex>
#include <sqlite3.h>

namespace haicam
{

    class UserDefault
    {
    private:
        UserDefault();

        static std::mutex mtx;
        static UserDefault *instance;

        sqlite3 *db;

    public:
        static UserDefault *getInstance();
        ~UserDefault();

        std::string getStringForKey(const std::string &key);
        void setStringForKey(const std::string &key, const std::string &value);
        int64_t getIntegerForKey(const std::string &key);
        void setIntegerForKey(const std::string &key, int64_t value);
        uint64_t getUIntegerForKey(const std::string &key);
        void setUIntegerForKey(const std::string &key, uint64_t value);
        bool getBoolForKey(const std::string &key);
        void setBoolForKey(const std::string &key, bool value);

        template<typename NONSTR>
        NONSTR getValueForKey(const std::string &key);

        template<typename NONSTR>
        void setValueForKey(const std::string &key, NONSTR value);

        void reset();
    };

}