#include <cassert>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
using namespace std;

#include "boost/format.hpp"

#include "SQLiteDB.h"
using namespace SQLite;

#include "Settings.h"
using namespace Viewer;

// todo: 
// - SQLite::DB is not threadfriendly. create a worker thread for db work?
// - replace direct queries with statements to protect against bad input

class SettingsCache
{
    enum class StoredType
    {
        String,
        Int,
        Double
    };
public:
    SettingsCache()
        : m_maxId(0)
    {}
    ~SettingsCache()
    {
        m_db.Close();
    }

    void AttachDB(const std::string & filename) 
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        m_maxId = 0;
        m_doubles.clear();
        m_ints.clear();
        m_ids.clear();
        m_strings.clear();

        m_db.Open(filename);
        m_db.ExecDML("CREATE TABLE IF NOT EXISTS Strings(Id TEXT,Value TEXT);");
        m_db.ExecDML("CREATE TABLE IF NOT EXISTS Integers(Id TEXT,Value INTEGER);");
        m_db.ExecDML("CREATE TABLE IF NOT EXISTS Doubles(Id TEXT,Value FLOAT);");

        auto sq = m_db.ExecQuery("SELECT Id, Value FROM Strings;");
        for (; !sq.IsEOF(); sq.NextRow())
        {
            std::string key = sq.GetStringField(0);
            std::string value = sq.GetStringField(1);
            m_ids.emplace(key, make_pair(StoredType::String,m_maxId));
            m_strings.emplace(m_maxId, value);
            ++m_maxId;
        }

        auto iq = m_db.ExecQuery("SELECT Id, Value FROM Integers;");
        for (; !iq.IsEOF(); iq.NextRow())
        {
            std::string key = iq.GetStringField(0);
            int value = iq.GetIntField(1);
            m_ids.emplace(key, make_pair(StoredType::Int,m_maxId));
            m_ints.emplace(m_maxId, value);
            ++m_maxId;
        }

        auto dq = m_db.ExecQuery("SELECT Id, Value FROM Doubles;");
        for (; !dq.IsEOF(); dq.NextRow())
        {
            std::string key = dq.GetStringField(0);
            double value = dq.GetFloatField(1);
            m_ids.emplace(key, make_pair(StoredType::Double,m_maxId));
            m_doubles.emplace(m_maxId, value);
            ++m_maxId;
        }
    }

    std::string GetString(const std::string& key, const std::string& def) 
    { 
        return GetValue(key, def);
    }
    int GetInt(const std::string& key, const int def)
    {
        return GetValue(key, def);
    }
    double GetDouble(const std::string& key, const double def)
    {
        return GetValue(key, def);
    }

    bool TryGetString(const std::string& key, std::string& value) 
    { 
        return TryGetValue(key,value);
    }
    bool TryGetInt(const std::string& key, int& value)
    {
        return TryGetValue(key, value);
    }
    bool TryGetDouble(const std::string& key, double& value) 
    {
        return TryGetValue(key, value);
    }

    void SetString(const std::string& key, const std::string& value)
    {
        SetValue(key, value);
    }
    void SetInt(const std::string& key, const int value)
    {
        SetValue(key, value);
    }
    void SetDouble(const std::string& key, const double value)
    {
        SetValue(key, value);
    }

protected:
    template<typename T>
    StoredType GetStoredType() { static_assert(false,"Unsupported StoredType"); }
    template<>
    StoredType GetStoredType<std::string>() { return StoredType::String; }
    template<>
    StoredType GetStoredType<int>() { return StoredType::Int; }
    template<>
    StoredType GetStoredType<double>() { return StoredType::Double; }

    template<typename T>
    const char* GetTable() { static_assert(false, "Unsupported StoredType"); }
    template<>
    const char* GetTable<std::string>() { return "Strings"; }
    template<>
    const char* GetTable<int>() { return "Integers"; }
    template<>
    const char* GetTable<double>() { return "Doubles"; }

    template<typename T>
    std::unordered_map<unsigned int, T>& GetContainer() { static_assert(false, "Unsupported StoredType"); }
    template<>
    std::unordered_map<unsigned int, std::string>& GetContainer<std::string>() { return m_strings; }
    template<>
    std::unordered_map<unsigned int, int>& GetContainer<int>() { return m_ints; }
    template<>
    std::unordered_map<unsigned int, double>& GetContainer<double>() { return m_doubles; }

    template<typename T>
    T GetValue(const std::string& key, const T& def)
    {
        T res;
        return TryGetValue(key, res) ? res : def;
    }

    template<typename T>
    bool TryGetValue(const std::string& key, T& value)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto id_iter = m_ids.find(key);
        if (id_iter != m_ids.end() &&
            id_iter->second.first == GetStoredType<T>())
        {
            auto& container = GetContainer<T>();
            auto key_iter = container.find(id_iter->second.second);
            if (key_iter != container.end())
            {
                value = key_iter->second;
                return true;
            }
        }
        return false;
    }

    template<typename T>
    void SetValue(const std::string& key, const T& value)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto id_iter = m_ids.find(key);
        if (id_iter != m_ids.end())
        {
            if (id_iter->second.first == GetStoredType<T>())
            {
                // Use existing slot
                GetContainer<T>()[id_iter->second.second] = value;
                m_db.ExecDML("UPDATE %1% SET Value = '%2%' WHERE Id = '%3%'", GetTable<T>(), value, key);
                return;
            }
            else
            {
                // Remove from current slot, it's the wrong type
                switch (id_iter->second.first)
                {
                case StoredType::Double:
                    m_doubles.erase(id_iter->second.second);
                    m_db.ExecDML("DELETE FROM Strings WHERE Id = '%1%'", key);
                    break;
                case StoredType::Int:
                    m_ints.erase(id_iter->second.second);
                    m_db.ExecDML("DELETE FROM Integers WHERE Id = '%1%'", key);
                    break;
                case StoredType::String:
                    m_strings.erase(id_iter->second.second);
                    m_db.ExecDML("DELETE FROM Doubles WHERE Id = '%1%'", key);
                    break;
                default:
                    assert(false); // new type?
                    break;
                }
                id_iter->second.first = GetStoredType<T>();
            }
        }
        else
        {
            id_iter = m_ids.emplace(key, make_pair(GetStoredType<T>(), m_maxId)).first;
            ++m_maxId;
        }
        GetContainer<T>().emplace(id_iter->second.second, value);
        m_db.ExecDML("INSERT INTO %1%(Id,Value) VALUES('%2%','%3%')", GetTable<T>(), key, value);
    }

private:
    SQLite::DB m_db;
    std::mutex m_mutex;
    std::unordered_map<std::string, std::pair<StoredType, unsigned int>> m_ids;
    std::unordered_map<unsigned int, std::string> m_strings;
    std::unordered_map<unsigned int, int> m_ints;
    std::unordered_map<unsigned int, double> m_doubles;
    unsigned int m_maxId;
};

namespace
{
    SettingsCache& Cache()
    {
        static SettingsCache cache;
        return cache;
    }
}; // freestanding functions

void Settings::AttachDB(const std::string & filename)
{
    Cache().AttachDB(filename);
}

std::string Settings::GetString(const std::string& key, const std::string& def)
{
    return Cache().GetString(std::move(key), std::move(def));
}
int Settings::GetInt(const std::string& key, const int def)
{
    return Cache().GetInt(std::move(key), def);
}
double Settings::GetDouble(const std::string& key, const double def)
{
    return Cache().GetDouble(std::move(key), def);
}
bool Settings::GetBool(const std::string& key, const bool def)
{
    return 1 == GetInt(std::move(key), def?1:0);
}

bool Settings::TryGetString(const std::string& key, std::string& value)
{
    return Cache().TryGetString(std::move(key), value);
}
bool Settings::TryGetInt(const std::string& key, int& value)
{
    return Cache().TryGetInt(std::move(key), value);
}
bool Settings::TryGetDouble(const std::string& key, double& value)
{
    return Cache().TryGetDouble(std::move(key), value);
}
bool Settings::TryGetBool(const std::string& key, bool& value)
{
    int valueI = 0;
    bool res = TryGetInt(std::move(key), valueI);
    value = (1 == valueI);
    return res;
}

void Settings::SetString(const std::string& key, const std::string& value)
{
    return Cache().SetString(std::move(key), std::move(value));
}
void Settings::SetInt(const std::string& key, const int value)
{
    return Cache().SetInt(std::move(key), value);
}
void Settings::SetDouble(const std::string& key, const double value)
{
    return Cache().SetDouble(std::move(key), value);
}
void Settings::SetBool(const std::string& key, const bool value)
{
    return SetInt(std::move(key), value ? 1 : 0);
}
