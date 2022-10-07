#include "haicam/UserDefault.hpp"
#include "haicam/Config.hpp"
#include "haicam/Utils.hpp"

using namespace haicam;

UserDefault *UserDefault::instance = NULL;
std::mutex UserDefault::mtx;

UserDefault::UserDefault() : db(NULL)
{
    char *zErrMsg = 0;

    std::string dbPath = Config::getInstance()->getUserWritablePath() + "/config";
    Utils::makeDir(dbPath);

#ifdef _WIN32
    dbPath += "\\user.default";
#else
    dbPath += "/user.default";
#endif

    int rc = sqlite3_open(dbPath.c_str(), &db);

    if (rc != SQLITE_OK)
    {

        Utils::log("sqlite: Cannot open database: %s", sqlite3_errmsg(db));
        sqlite3_close(db);

        db = NULL;

        H_ASSERT_ERR_STR("sqlite: Cannot open database");
    }
    else
    {
        sqlite3_extended_result_codes(db, 1);

        const char *sql = "CREATE TABLE IF NOT EXISTS user_default ("
                          "key text PRIMARY KEY,"
                          "value text not null,"
                          "updated_at integer not null"
                          ");";
        rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            Utils::log("sqlite: error: %s", zErrMsg);
            sqlite3_free(zErrMsg);
            sqlite3_close(db);
            db = NULL;
            H_ASSERT_ERR_STR("sqlite SQL: CREATE TABLE IF NOT EXISTS user_default  error");
        }
        else
        {
            Utils::log("sqlite: Table created successfully");
        }
    }
}

UserDefault *UserDefault::getInstance()
{
    if (instance == NULL)
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (instance == NULL)
        {
            instance = new UserDefault();
        }

        H_ASSERT(instance->db != NULL);
    }

    return instance;
}

UserDefault::~UserDefault()
{
    instance = NULL;
}

std::string UserDefault::getStringForKey(const std::string &key)
{
    std::lock_guard<std::mutex> lock(mtx);

    std::string ret;

    sqlite3_stmt *pStmt;
    const char *sql = "SELECT value FROM user_default WHERE key = ?";
    if (sqlite3_prepare_v2(db, sql, -1, &pStmt, 0) != SQLITE_OK)
    {
        Utils::log("UserDefault::getStringForKey prepare statement err: %s\n", sqlite3_errmsg(db));
        return ret;
    }
    sqlite3_bind_text(pStmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(pStmt) == SQLITE_ROW)
    {
        ret = (const char *)sqlite3_column_text(pStmt, 0);
    }

    sqlite3_finalize(pStmt);

    return ret;
}

void UserDefault::setStringForKey(const std::string &key, const std::string &value)
{
    std::lock_guard<std::mutex> lock(mtx);

    sqlite3_stmt *pStmt;
    const char *sql = "INSERT INTO user_default (key, value, updated_at)"
                      "VALUES(?, ?, ?) ON CONFLICT (key) DO UPDATE SET value = ?, updated_at = ?";
    if (sqlite3_prepare_v2(db, sql, -1, &pStmt, 0) != SQLITE_OK)
    {
        Utils::log("UserDefault::setStringForKey prepare statement err: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_text(pStmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(pStmt, 2, value.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(pStmt, 3, time(NULL));
    sqlite3_bind_text(pStmt, 4, value.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(pStmt, 5, time(NULL));

    int code = sqlite3_step(pStmt);

    if (code != SQLITE_DONE)
    {
        int exCode = sqlite3_extended_errcode(db);
        Utils::log("UserDefault::setStringForKey db error: %i, %i, %s", code, exCode, sqlite3_errmsg(db));
    }
    sqlite3_finalize(pStmt);
}

int64_t UserDefault::getIntegerForKey(const std::string &key)
{
    std::string strVal = getStringForKey(key);
    return Utils::fromStrTo<int64_t>(strVal);
}

void UserDefault::setUIntegerForKey(const std::string &key, uint64_t value)
{
    std::string strVal = Utils::toStr<uint64_t>(value);
    setStringForKey(key, strVal);
}

uint64_t UserDefault::getUIntegerForKey(const std::string &key)
{
    std::string strVal = getStringForKey(key);
    return Utils::fromStrTo<uint64_t>(strVal);
}

void UserDefault::setIntegerForKey(const std::string &key, int64_t value)
{
    std::string strVal = Utils::toStr<int64_t>(value);
    setStringForKey(key, strVal);
}

bool UserDefault::getBoolForKey(const std::string &key)
{
    std::string strVal = getStringForKey(key);
    if (strVal == "" || strVal == "0")
        return false;
    else
        return true;
}

void UserDefault::setBoolForKey(const std::string &key, bool value)
{
    std::string strVal = value ? "1" : "0";
    setStringForKey(key, strVal);
}

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

void UserDefault::reset()
{
    std::lock_guard<std::mutex> lock(mtx);

    sqlite3_stmt *pStmt;
    const char *sql = "DELETE FROM user_default WHERE 1;";

    if (sqlite3_prepare_v2(db, sql, -1, &pStmt, 0) != SQLITE_OK)
    {
        Utils::log("UserDefault::reset statement err: %s\n", sqlite3_errmsg(db));
        return;
    }

    if (sqlite3_step(pStmt) != SQLITE_DONE)
    {
        Utils::log("UserDefault::reset !SQLITE_DONE");
    }
    sqlite3_finalize(pStmt);
}
