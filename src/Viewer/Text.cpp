#include <string>
using namespace std;

#include "freetype/ft2build.h"

#include "Geometry.h"
#include "GLWrappers.h"
#include "Text.h"
using namespace Viewer;

Text::Text(std::string&& text,const double size)
    : m_text(std::move(text))
    , m_size(size)
{}

Text& Text::Size(const double size)
{
    m_size = size;
    return *this;
}

void Text::Draw(const double x, const double y)
{}

