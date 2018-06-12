#pragma once

namespace Viewer
{
    class Data
    {
    public:
        static std::vector<unsigned char> GetFont(const std::string& fontName);
        static std::vector<std::string> GetFontNames();
    };
}; // namespace Viewer

