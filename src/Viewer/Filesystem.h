#pragma once

namespace Viewer
{
    class Filesystem
    {
    public:
        static std::string GetUserPath();
        static std::string GetSettingsFilepath();
        static std::string GetDataFilepath();
    };
}; // namespace Viewer
