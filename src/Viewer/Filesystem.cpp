#include <string>
#include <unordered_map>
#include <algorithm>
#include <memory>
using namespace std;

#include "boost/filesystem.hpp"

#include "SQLiteDB.h"
using namespace SQLite;

#include "Geometry.h"
#include "Filesystem.h"
using namespace Viewer;

#ifdef _WIN32
  #include "windows.h"
#else // _WIN32

no linux / unix specific headers yet

#endif // _WIN32


std::string Filesystem::GetUserPath()
{
    char const* home = getenv("HOME");
    if (home || (home = getenv("USERPROFILE")))
    {
        return home;
    }
    else
    {
        char const *hdrive = getenv("HOMEDRIVE");
        char const *hpath = getenv("HOMEPATH");
        return std::string(hdrive) + hpath;
    }
}

std::string Filesystem::GetSettingsFilepath()
{
    return GetUserPath() + "/3DModelingViewer.settings";
}

std::string Filesystem::GetDataFilepath()
{
    namespace fs = boost::filesystem;

    std::string executable;
#ifdef _WIN32
    char buffer[_MAX_PATH];
    GetModuleFileNameA(NULL, buffer, sizeof(buffer));
    executable = buffer;
#else // _WIN32

    no linux / unix code yet

        readlink("/proc/self/exe", buf, bufsize) (Linux)

        readlink("/proc/curproc/file", buf, bufsize) (FreeBSD)

        readlink("/proc/self/path/a.out", buf, bufsize) (Solaris)


#endif // _WIN32
    auto fullPath = fs::canonical(executable);
    return fullPath.replace_extension(".data").string();
}
