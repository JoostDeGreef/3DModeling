#include <string>
#include <unordered_map>
#include <algorithm>
using namespace std;

#include "ft2build.h"
#include FT_FREETYPE_H

#include "Geometry.h"
#include "GLWrappers.h"
#include "Text.h"
using namespace Viewer;

class FreeType
{
public:
    FreeType()
    {
        if (FT_Init_FreeType(&m_ft) != 0) 
        {
// todo: exception?
//            fprintf(stderr, "Could not init freetype library\n");
//            return 1;
        }        
    }

    ~FreeType()
    {
        FT_Done_FreeType(m_ft);
    }

    operator FT_Library&() { return m_ft; }

private:
    FT_Library m_ft;
};

class Font
{
public:
    Font(const std::string& fontfile,const unsigned int size)
    {
        if (FT_New_Face(m_ft, fontfile.c_str(), 0, &m_face) != 0) 
        {
// todo: exception?
//            fprintf(stderr, "Could not open font\n");
//            return 1;
        }
        FT_Set_Pixel_Sizes(m_face, 0, size);
    }
    ~Font()
    {
        FT_Done_Face(m_face);
    }

    operator FT_Face&() { return m_face; }

private:
    static FreeType m_ft;
    FT_Face m_face;
};

FreeType Font::m_ft;

class Character
{
public:
    Character(const char c,
              const int width, const int height,
              const int left, const int top,
              const int advance_x, const int advance_y)
        : m_char(c)
        , m_width(width)
        , m_height(height)
        , m_left(left)
        , m_top(top)
        , m_advance_x(advance_x)
        , m_advance_y(advance_y)
    {}

    operator char() { return m_char; }

    int GetHeight() const { return m_height; }
    int GetWidth() const { return m_width; }
private:
    char m_char;
    int m_width;
    int m_height;
    int m_left;
    int m_top;
    int m_advance_x;
    int m_advance_y;
    int m_texture_x;
    int m_texture_y;
};

class Atlas
{
public:
    Atlas(const std::string& fontfile,const unsigned int size)
        : m_font(fontfile,size)
    {
        LoadCharacters(" "
                       "0123456789"
                       "abcdefghijklmnopqrstuvwxyz"
                       "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                       "!@#$%^&*()-_=+`~[]{};:'\"\\|,.<>/?");
    }

protected:
    void LoadCharacters(const std::string& chars)
    {
        FT_Face face = m_font;
        FT_GlyphSlot g = face->glyph;
        int w = 0;
        int h = 0;
        int64_t surface = 0;
        std::vector<Character> characters;

        for (char c: chars) 
        {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER)) 
            {
//                fprintf(stderr, "Loading character %c failed!\n", i);
                continue;
            }
            surface += g->bitmap.width * g->bitmap.rows;
            characters.emplace_back(c, g->bitmap.width, g->bitmap.rows, g->bitmap_left, g->bitmap_top, g->advance.x, g->advance.y);
        }

        sort(characters.begin(), characters.end(), [](Character& a, Character& b) { return a.GetHeight() > b.GetHeight(); });

        // todo: fit on m*m texture

        for (Character& c : characters)
        {
            m_characters.emplace(c, c);
        }
    }

    Character& GetCharacter(char c)
    {
        auto iter = m_characters.find(c);
        if (iter == m_characters.end())
        {
            iter = m_characters.find('?');
            assert(iter != m_characters.end());
        }
        return iter->second;
    }

private:
    Font m_font;
    std::unordered_map<char, Character> m_characters;
};


// todo: store data somewhere logical...
Atlas& GetAtlas(const std::string& fontfile, const unsigned int size = 48)
{
    static std::unordered_map<std::string,Atlas> atlasses;
    std::string id = std::to_string(size) + ";" + fontfile;
    auto iter = atlasses.find(id);
    if (iter == atlasses.end())
    {
        iter = atlasses.emplace(id, Atlas(fontfile, size)).first;
    }
    return iter->second;
}


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
{
    Atlas& atlas = GetAtlas(R"TEXT(C:\Src\3DModeling\src\Viewer\fonts\Prida65.otf)TEXT");
}

