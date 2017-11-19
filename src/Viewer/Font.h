#pragma once

namespace Viewer
{
    class Font
    {
    public:
        Font(std::string&& fontFile, const int size = 12);

        Font& PixelSize(const double pixelSize);
        Font& Color(const Geometry::Color& color);

        void Draw(const double x,const double y, const std::string& text);
        Geometry::Vector2d GetSize(const std::string& text);

    private:
        std::string m_fontFile;
        int m_size;
        double m_pixelSize;
        Geometry::Color m_color;
    };
}; // namespace Viewer

