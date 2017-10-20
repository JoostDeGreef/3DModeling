#ifndef SQLITESTATEMENTSTATE_H
#define SQLITESTATEMENTSTATE_H 1

class Statement::State
{
public:
    State(sqlite3* db, sqlite3_stmt* statement,bool eof,int columnCount = -1)
        : m_db(db)
        , m_statement(statement)
        , m_eof(eof)
        , m_columnCount(columnCount)
    {
        if (-1 == m_columnCount)
        {
            m_columnCount = sqlite3_column_count(m_statement);
        }
    }

    void Check()
    {
        if (nullptr == m_db)
        {
            ThrowError("Database not open");
        }
        if (nullptr == m_statement)
        {
            ThrowError("Statement not active");
        }
    }

    sqlite3_stmt* m_statement;
    sqlite3* m_db;
    bool m_eof;
    int m_columnCount;
};

#endif // SQLITESTATEMENTSTATE_H

