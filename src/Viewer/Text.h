#pragma once

namespace Viewer
{
    class Text
    {
    public:
        Text(std::string&& text, const int size = 12);

        Text& Size(const int size, const double pixelSize);
        void Draw(const double x,const double y);
        Geometry::Vector2i GetSize();
    private:
        std::string m_text;
        int m_size;
        double m_pixelSize;
    };
}; // namespace Viewer

