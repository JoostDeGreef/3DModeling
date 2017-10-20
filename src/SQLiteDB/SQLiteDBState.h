#ifndef SQLITEDBSTATE_H
#define SQLITEDBSTATE_H 1

class DB::State
{
public:
    State()
        : m_db(nullptr)
        , m_busyTimeoutMs(6000)
    {}

    void Check()
    {
        if (nullptr == m_db)
        {
//        throw CppSQLite3Exception(nRet, "DB not opened");
        }
    }

    sqlite3_stmt* Compile(const std::string& sql)
    {
        Check();

        char* szError = 0;
        const char* szTail = 0;
        sqlite3_stmt* pVM;

        int nRet = sqlite3_prepare(m_db, sql.c_str(), -1, &pVM, &szTail);

        if (nRet != SQLITE_OK)
        {
//            throw CppSQLite3Exception(nRet, szError);
        }

        return pVM;
    }

    sqlite3* m_db;
    int m_busyTimeoutMs;
};

#endif // SQLITEDBSTATE_H
