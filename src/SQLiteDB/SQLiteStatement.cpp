#include <memory>
#include <string>
#include <vector>

using namespace std;

#include "sqlite/sqlite3.h"
#include "SQLiteDB.h"

using namespace SQLite;

#include "SQLiteSupport.h"
#include "SQLiteStatementState.h"
#include "SQLiteQueryState.h"

Statement::Statement(std::unique_ptr<State>&& state)
    : m_state(std::move(state))
{}

Statement::Statement(Statement&& statement)
    : m_state(std::move(statement.m_state))
{}

Statement::~Statement()
{
    Finalize();
}

int Statement::ExecDML()
{
    m_state->Check();

    int rowsChanged = 0;
    int ret = sqlite3_step(m_state->m_statement);

    if (ret == SQLITE_DONE)
    {
        rowsChanged = sqlite3_changes(m_state->m_db);

        ret = sqlite3_reset(m_state->m_statement);

        if (SQLITE_OK != ret)
        {
            ThrowError(m_state->m_db,ret);
        }
    }
    else
    {
        ret = sqlite3_reset(m_state->m_statement);
        ThrowError(m_state->m_db, ret);
    }
    return rowsChanged;
}

Query Statement::ExecQuery()
{
    m_state->Check();

    int ret = sqlite3_step(m_state->m_statement);

    if (ret == SQLITE_DONE)
    {
        // no rows
        return std::make_unique<Query::State>(m_state->m_db, m_state->m_statement, true, false);
    }
    else if (ret == SQLITE_ROW)
    {
        // at least 1 row
        return std::make_unique<Query::State>(m_state->m_db, m_state->m_statement, false, false);
    }
    else
    {
        ret = sqlite3_reset(m_state->m_statement);
        ThrowErrorIfNotOK(m_state->m_db, ret);
        return nullptr;
    }
}

void Statement::Bind(int param, const std::string& value)
{
    Bind(param, value.c_str(), static_cast<int>(value.size()));
}

void Statement::Bind(int param, const char* value, const int length)
{
    m_state->Check();
    int ret = sqlite3_bind_text(m_state->m_statement, param, value, length, SQLITE_TRANSIENT);
    ThrowErrorIfNotOK("Error binding string param", ret);
}

void Statement::Bind(int param, const int value)
{
    m_state->Check();
    int ret = sqlite3_bind_int(m_state->m_statement, param, value);
    ThrowErrorIfNotOK("Error binding int param", ret);
}

void Statement::Bind(int param, const long long value)
{
    m_state->Check();
    int ret = sqlite3_bind_int64(m_state->m_statement, param, value);
    ThrowErrorIfNotOK("Error binding int64 param", ret);
}

void Statement::Bind(int param, const double value)
{
    m_state->Check();
    int ret = sqlite3_bind_double(m_state->m_statement, param, value);
    ThrowErrorIfNotOK("Error binding double param", ret);
}

void Statement::Bind(int param, const unsigned char* value, int length)
{
    m_state->Check();
    int ret = sqlite3_bind_blob(m_state->m_statement, param, (const void*)value, length, SQLITE_TRANSIENT);
    ThrowErrorIfNotOK("Error binding blob param", ret);
}

void Statement::BindNull(int param)
{
    m_state->Check();
    int ret = sqlite3_bind_null(m_state->m_statement, param);
    ThrowErrorIfNotOK("Error binding NULL param", ret);
}

void Statement::Reset()
{
    if (m_state->m_statement)
    {
        int ret = sqlite3_reset(m_state->m_statement);
        ThrowErrorIfNotOK(m_state->m_db, ret);
    }
}

void Statement::Finalize()
{
    if (m_state->m_statement)
    {
        int ret = sqlite3_finalize(m_state->m_statement);
        m_state->m_statement = nullptr;
        ThrowErrorIfNotOK(m_state->m_db, ret);
    }
}
