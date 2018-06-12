#include <string>
#include <unordered_map>
#include <algorithm>
#include <memory>
using namespace std;

#include "SQLiteDB.h"
using namespace SQLite;

#include "Geometry.h"
#include "Data.h"
#include "Filesystem.h"
using namespace Viewer;

class DataCore
{
public:
    DataCore()
    {
        m_db.Open(Filesystem::GetDataFilepath(), true);
    }
    std::vector<unsigned char> GetFont(const std::string& fontName)
    {
        auto q = m_db.ExecQuery("SELECT Data FROM Fonts WHERE Name = '%1%'", fontName);
        return q.GetBlobField(0);
    }
    std::vector<std::string> GetFontNames()
    {
        std::vector<std::string> res;
        auto q = m_db.ExecQuery("SELECT Name FROM Fonts;");
        for (; !q.IsEOF(); q.NextRow())
        {
            res.emplace_back(q.GetStringField(0));
        }
        return res;
    }
private:
    SQLite::DB m_db;
};

namespace
{
    DataCore& GetDataCore()
    {
        thread_local static DataCore dc;
        return dc;
    }
}

std::vector<unsigned char> Data::GetFont(const std::string& fontName)
{
    return GetDataCore().GetFont(fontName);
}
std::vector<std::string> Data::GetFontNames()
{
    return GetDataCore().GetFontNames();
}
