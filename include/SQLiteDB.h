#pragma once

namespace SQLite
{
    enum class DataType
    {
        Integer,
        Float,
        Blob,
        Null,
        Text,
    };

    class Query
    {
    public:
        class State;
    public:
        Query();
        Query(std::shared_ptr<State>&& state);
        Query(Query&& query);
        Query(const Query& query);
        ~Query();

        Query& operator = (const Query& query);

        int GetFieldCount() const;
        const std::vector<std::string>& GetFieldNames() const;
        int FieldIndex(const std::string& fieldName) const;
        const std::string& FieldName(int field) const;
        std::string FieldDeclType(int field) const;
        DataType FieldDataType(int field) const;
        std::string FieldValue(int field) const;
        std::string FieldValue(const std::string& field) const;
        int GetIntField(int field, int nullValue = 0) const;
        int GetIntField(const std::string& field, int nullValue = 0) const;
        int64_t GetInt64Field(int field, long long nullValue = 0) const;
        int64_t GetInt64Field(const std::string& field, long long nullValue = 0) const;
        double GetFloatField(int field, double nullValue = 0.0) const;
        double GetFloatField(const std::string& field, double nullValue = 0.0) const;
        std::string GetStringField(int field, const std::string& nullValue = "") const;
        std::string GetStringField(const std::string& field, const std::string& nullValue = "") const;
        std::vector<unsigned char> GetBlobField(int field) const;
        std::vector<unsigned char> GetBlobField(const std::string& field) const;
        bool FieldIsNull(int field) const;
        bool FieldIsNull(const std::string& field) const;
        bool IsEOF() const;
        void NextRow();
        void Finalize();
    private:
        std::shared_ptr<State> m_state;
    };

    class Statement
    {
    public:
        class State;
    public:
        Statement();
        Statement(std::shared_ptr<State>&& state);
        Statement(Statement&& statement);
        Statement(const Statement& statement);
        ~Statement();

        Statement& operator = (const Statement& statement);

        int ExecDML();
        Query ExecQuery();
        void Bind(int param, const std::string& value);
        void Bind(int param, const char* value, const int length = -1);
        void Bind(int param, const int value);
        void Bind(int param, const long long value);
        void Bind(int param, const double value);
        void Bind(int param, const unsigned char* value, int length);
        void BindNull(int param);
        void Reset();
        void Finalize();
    private:
        std::shared_ptr<State> m_state;
    };

    class DB
    {
    public:
        class State;
    public:
        DB();
        ~DB();

        void Open(const std::string& fileName);
        void Close();
        bool TableExists(const std::string& tableName);

        int ExecDML(const std::string& sql);
        template<typename T0,typename... TN>
        int ExecDML(const std::string& fmt, T0&& t0, TN&&... tn)
        {
            return ExecDML(FormatArgs(fmt,t0,tn...));
        }

        int64_t ExecSingleInt64(const std::string& sql);
        template<typename T0, typename... TN>
        int64_t ExecSingleInt64(const std::string& fmt, T0&& t0, TN&&... tn)
        {
            return ExecSingleInt64(FormatArgs(fmt, t0, tn...));
        }

        std::string ExecSingleString(const std::string& sql);
        template<typename T0, typename... TN>
        std::string ExecSingleString(const std::string& fmt, T0&& t0, TN&&... tn)
        {
            return ExecSingleString(FormatArgs(fmt, t0, tn...));
        }

        Query ExecQuery(const std::string& sql);
        template<typename T0, typename... TN>
        Query ExecQuery(const std::string& fmt, T0&& t0, TN&&... tn)
        {
            return ExecQuery(FormatArgs(fmt, t0, tn...));
        }

        Statement CompileStatement(const std::string& sql);
        template<typename T0, typename... TN>
        Statement CompileStatement(const std::string& fmt, T0&& t0, TN&&... tn)
        {
            return CompileStatement(FormatArgs(fmt, t0, tn...));
        }

        void BeginTransaction();
        void CommitTransaction();

        int64_t LastRowId() const;
        void Interrupt();
        void SetBusyTimeout(int milliSecs);
        static std::string SQLiteVersion();
    private:
        template<typename... Arguments>
        std::string FormatArgs(const std::string& fmt, const Arguments&... args)
        {
            boost::format f(fmt);
            std::initializer_list<char> {(static_cast<void>(f % args), char{}) ...};
            return boost::str(f);
        }

        std::shared_ptr<State> m_state;
    };
} // namespace SQLite

