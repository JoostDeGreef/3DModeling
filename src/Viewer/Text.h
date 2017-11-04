#pragma once

namespace Viewer
{
    class Text
    {
    public:
        Text(std::string&& text, const double size = 12);

        Text& Size(const double size);
        void Draw(const double x,const double y);
    private:
        std::string m_text;
        double m_size;
    };
}; // namespace Viewer

