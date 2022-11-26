#pragma once
#include <string>
#include <mutex>
#include <sqlite3.h>
#include "haicam/Utils.hpp"

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

        template <typename NONSTR>
        NONSTR getValueForKey(const std::string &key);

        template <typename NONSTR>
        void setValueForKey(const std::string &key, NONSTR value);

        void reset();

        // shortcut
        std::string getIceServers()
        {
            return getStringForKey("ice_servers");
        }

        uint32_t getLocalAddr()
        {
            return getValueForKey<uint32_t>("local_addr");
        }

        std::string getRSAKey1024() { return getStringForKey("rsa_key_1014"); }
        std::string getAESKey128() { return getStringForKey("aes_key_128"); }
        std::string getRSAKey2048() { return getStringForKey("rsa_key_2048"); }
        std::string getAESKey256() { return getStringForKey("aes_key_256"); }
        std::string getServerRSAKey2048() { return getStringForKey("server_rsa_key_2048"); }
    };

    /*
    templated classes/functions cannot be separated as a header + implementation file 
    */
    template<typename NONSTR>
    NONSTR UserDefault::getValueForKey(const std::string &key)
    {
        std::string strVal = getStringForKey(key);
        return Utils::fromStrTo<NONSTR>(strVal);
    }

    template<typename NONSTR>
    void UserDefault::setValueForKey(const std::string &key, NONSTR value)
    {
        std::string strVal = Utils::toStr<NONSTR>(value);
        setStringForKey(key, strVal);
    }

}