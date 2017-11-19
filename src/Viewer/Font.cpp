#include <string>
#include <unordered_map>
#include <algorithm>
using namespace std;

#include "ft2build.h"
#include FT_FREETYPE_H

#include "Geometry.h"
#include "GLWrappers.h"
#include "Font.h"
#include "Shaders.h"
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

class FreeTypeFont
{
public:
    FreeTypeFont(const std::string& fontfile,const unsigned int size)
    {
        if (FT_New_Face(m_ft, fontfile.c_str(), 0, &m_face) != 0) 
        {
// todo: exception?
//            fprintf(stderr, "Could not open font\n");
//            return 1;
        }
        FT_Set_Pixel_Sizes(m_face, 0, size);
    }
    ~FreeTypeFont()
    {
        FT_Done_Face(m_face);
    }

    operator FT_Face&() { return m_face; }

private:
    static FreeType m_ft;
    FT_Face m_face;
};

FreeType FreeTypeFont::m_ft;

class Character
{
public:
    Character(const char c,
              const int width, const int height,
              const int left, const int top,
              const int advance_x, const int advance_y,
              const int bearing_x, const int bearing_y)
        : m_char(c)
        , m_width(width)
        , m_height(height)
        , m_left(left)
        , m_top(top)
        , m_advance_x(advance_x)
        , m_advance_y(advance_y)
        , m_bearing_x(bearing_x)
        , m_bearing_y(bearing_y)
    {}

    operator char() const { return m_char; }

    int GetHeight() const { return m_height; }
    int GetWidth() const { return m_width; }

    int GetTextureX() const { return m_texture_x; }
    int GetTextureY() const { return m_texture_y; }

    int GetAdvanceX() const { return m_advance_x; }
    int GetAdvanceY() const { return m_advance_y; }

    int GetBearingX() const { return m_bearing_x; }
    int GetBearingY() const { return m_bearing_y; }

    void SetTexturePos(const int x, const int y) { m_texture_x = x; m_texture_y = y; }
private:
    char m_char;
    int m_width;
    int m_height;
    int m_left;
    int m_top;
    int m_advance_x;
    int m_advance_y;
    int m_bearing_x;
    int m_bearing_y;
    int m_texture_x;
    int m_texture_y;
};

class CharacterMap
{
public:
    CharacterMap(const std::string& fontfile,const unsigned int size)
        : m_characters()
        , m_textureId(0)
    {
        FreeTypeFont font(fontfile, size);
        LoadCharacters(font,
                       " "
                       "0123456789"
                       "abcdefghijklmnopqrstuvwxyz"
                       "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                       "!@#$%^&*()-_=+`~[]{};:'\"\\|,.<>/?");
    }

    void RenderText(const std::string& text, double x, double y, double scale_x, double scale_y, Geometry::Color& color)
    {
        Shaders::Type prevShader = Shaders::Select(Shaders::Type::Text);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_textureId); 

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int inputColorLocation = Shaders::GetUniformLocation("inputColor");
        int textureLocation = Shaders::GetUniformLocation("texture");

        glUniform1i(textureLocation, 0); // sampler, not texture!
        glUniform4fv(inputColorLocation, 1, color.GetRGBA());

        glBegin(GL_TRIANGLES);
        for (const char c: text)
        {
            const Character& character = GetCharacter(c);

            double tx0 = character.GetTextureX() * m_scale;
            double tx1 = (character.GetTextureX() + character.GetWidth()) * m_scale;
            double ty0 = character.GetTextureY() * m_scale;
            double ty1 = (character.GetTextureY() + character.GetHeight()) * m_scale;

            double w = character.GetWidth() * scale_x;
            double h = character.GetHeight() * scale_y;
            double bx = character.GetBearingX() * scale_x / 64;
            double by = character.GetBearingY() * scale_y / 64;

            /* Skip glyphs that have no pixels */
            if (w && h)
            {
                glTexCoord2d(tx0, ty1);
                glVertex2d(x + bx, y - h + by);
                glTexCoord2d(tx1, ty0);
                glVertex2d(x + w + bx, y + by);
                glTexCoord2d(tx0, ty0);
                glVertex2d(x + bx, y + by);

                glTexCoord2d(tx1, ty1);
                glVertex2d(x + w + bx, y - h + by);
                glTexCoord2d(tx1, ty0);
                glVertex2d(x + w + bx, y + by);
                glTexCoord2d(tx0, ty1);
                glVertex2d(x + bx, y - h + by);
            }

            /* Advance the cursor to the start of the next character */
            x += character.GetAdvanceX() * scale_x / 64;
            y += character.GetAdvanceY() * scale_y / 64;
        }
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);
        Shaders::Select(prevShader);
    }

    // this ignores whatever falls below the baseline of the glyph
    Geometry::Vector2d MeasureText(const std::string& text, double scale_x, double scale_y)
    {
        double x = 0;
        double y = 0;
        for (const char c : text)
        {
            const Character& character = GetCharacter(c);

            double h = character.GetHeight() * scale_y;
            x += character.GetAdvanceX() * scale_x / 64;

            y = std::max(y, h);
        }
        Geometry::Vector2d res(x,y);
        return res;
    }

protected:
    void LoadCharacters(FreeTypeFont& font, const std::string& chars)
    {
        FT_Face face = font;
        FT_GlyphSlot g = face->glyph;
        int64_t surface = 0;
        std::vector<Character> characters;

        for (char c: chars) 
        {
            if (0 != FT_Load_Char(face, c, FT_LOAD_RENDER)) 
            {
//                fprintf(stderr, "Loading character %c failed!\n", i);
                continue;
            }
            surface += g->bitmap.width * g->bitmap.rows;
            characters.emplace_back(c, g->bitmap.width, g->bitmap.rows, g->bitmap_left, g->bitmap_top, g->advance.x, g->advance.y, g->metrics.horiBearingX, g->metrics.horiBearingY);
        }

        // sort the glyps with the tallest ones in front
        sort(characters.begin(), characters.end(), [](Character& a, Character& b) 
        { 
            return a.GetHeight() == b.GetHeight() ? a.GetWidth() > b.GetWidth() : a.GetHeight() > b.GetHeight();
        });

        // see how big the (square) texture should be to contain all glyphs.
        int side = (int)pow(2, ceil(log2(sqrt(surface)/2)));
        int height;
        do
        {
            side *= 2;
            height = 0;
            int x = 0;
            int y = 0;
            for (Character& c : characters)
            {
                if (c.GetWidth() + x + 2> side)
                {
                    x = 0;
                }
                if (x == 0)
                {
                    y = height;
                    height += c.GetHeight() + 2;
                }
                c.SetTexturePos(x+1,y+1);
                x += c.GetWidth() + 2;
            }
        }
        while (height>side);

        // create the texture
        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &m_textureId);
        glBindTexture(GL_TEXTURE_2D, m_textureId);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, side, side, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

        for (const Character& c : characters)
        {
            if (0 == FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                glTexSubImage2D(GL_TEXTURE_2D, 0, c.GetTextureX(), c.GetTextureY(),
                    g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);
            }
            else
            {
                int dummy = 0;
            }
        }
        glBindTexture(GL_TEXTURE_2D, 0);

        // store the characters
        for (Character& c : characters)
        {
            m_characters.emplace(c, c);
        }

        // store 1/side for texture coord normalization
        m_scale = 1.0 / side;
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
    std::unordered_map<char, Character> m_characters;
    unsigned int m_textureId;
    double m_scale;
};


// todo: store data somewhere logical...
CharacterMap& GetCharacterMap(const std::string& fontfile, const unsigned int size)
{
    static std::unordered_map<std::string, CharacterMap> characterMaps;
    std::string id = std::to_string(size) + ";" + fontfile;
    auto iter = characterMaps.find(id);
    if (iter == characterMaps.end())
    {
        iter = characterMaps.emplace(id, CharacterMap(fontfile, size)).first;
    }
    return iter->second;
}


Font::Font(std::string&& fontFile, const int size)
    : m_fontFile(std::move(fontFile))
    , m_size(size)
    , m_pixelSize(1/800)
    , m_color(1,1,1,1)
{}

Font& Font::PixelSize(const double pixelSize)
{
    m_pixelSize = pixelSize;
    return *this;
}

Font& Font::Color(const Geometry::Color& color)
{
    m_color = color;
    return *this;
}

void Font::Draw(const double x, const double y, const std::string& text)
{
    CharacterMap& characterMap = GetCharacterMap(m_fontFile, m_size);
    characterMap.RenderText(std::move(text), x, y, m_pixelSize, m_pixelSize, m_color);
}

Geometry::Vector2d Font::GetSize(const std::string& text)
{
    CharacterMap& characterMap = GetCharacterMap(m_fontFile, m_size);
    return characterMap.MeasureText(std::move(text), m_pixelSize, m_pixelSize);
}
