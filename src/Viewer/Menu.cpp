#include "Geometry.h"
#include "Font.h"
#include "Menu.h"
#include "GLWrappers.h"

namespace Viewer
{
    Menu::Menu(const Font& font)
        : m_prevWidth(0)
        , m_prevHeight(0)
        , m_displayList(0)
        , m_prevOpacity(1.0)
        , m_font(font)
    {}

    void Menu::Draw(int width, int height, double X, double Y)
    {
        if (m_state == MenuState::None)
        {
            DrawHamburger(width, height, X, Y);
        }
        else
        {
            DrawExpanded(width, height, X, Y);
        }
    }
    void Menu::DrawHamburger(int width, int height, double X, double Y)
    {
        // menu opacity changes on mousehover
        double opacity = 0.5;
        auto mousePos = Geometry::Vector2d(m_mousePos[0]*2.0*X/width - X, Y - m_mousePos[1]*2.0*Y/height);
        if (m_bboxHamburger.IsInitialized() && m_bboxHamburger.Encapsulates(mousePos))
        {
            opacity = 1.0;
        }
        m_font.Color(Geometry::Color::Red()).Draw(0, 0, std::to_string(mousePos[0]) + " x " + std::to_string(mousePos[1]));

        glColor4d(1, 1, 1, opacity);

        if (width != m_prevWidth ||
            height != m_prevHeight || 
            opacity != m_prevOpacity )
        {
            m_prevWidth = width;
            m_prevHeight = height;
            m_prevOpacity = opacity;

            glDeleteLists(m_displayList, 1);
            m_displayList = glGenLists(1);
            glNewList(m_displayList, GL_COMPILE);

            // size of one pixel
            double pixel = X / width;

            // start position
            double x = -X + 5 * pixel;
            double y = Y - 5 * pixel;

            int bh = 15; // one bar heigth
            int gh = 8;  // gap height

            for (int i = 0; i < 3; ++i)
            {
                glBegin(GL_QUADS);
                    glVertex2d(x + 0*bh*pixel, y - ((bh + gh)*i + 0)*pixel);
                    glVertex2d(x + 0*bh*pixel, y - ((bh + gh)*i + bh)*pixel);
                    glVertex2d(x + (bh*3 + gh*2)*pixel, y - ((bh + gh)*i + bh)*pixel);
                    glVertex2d(x + (bh*3 + gh*2)*pixel, y - ((bh + gh)*i + 0)*pixel);
                glEnd();
            }

            glEndList();

            m_bboxHamburger.Set(Geometry::Vector2d(x + 0 * bh*pixel, y - ((bh + gh)*0 + 0)*pixel), 
                                Geometry::Vector2d(x + (bh * 3 + gh * 2)*pixel, y - ((bh + gh)*2 + bh)*pixel));
        }
        glCallList(m_displayList);
    }
    void Menu::DrawExpanded(int width, int height, double X, double Y)
    {

    }

    bool Menu::HandleKey(const int key, const int scancode, const int action, const int mods)
    {
        // esc: key = GLFW_KEY_ESCAPE, scancode = 1, action = GLFW_PRESS, mods = 0
        if (m_state == MenuState::None)
        {
            if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            {
                m_state = MenuState::Opened;
            }
        }
        else
        {
            if (action == GLFW_PRESS)
            {
                switch (key)
                {
                case GLFW_KEY_ESCAPE:
                    m_state = MenuState::None;
                    break;
                case GLFW_KEY_ENTER:
                    break;
                case GLFW_KEY_DOWN:
                    break;
                case GLFW_KEY_UP:
                    break;
                case GLFW_KEY_LEFT:
                    break;
                case GLFW_KEY_RIGHT:
                    break;
                }
            }
        }
        return false;
    }

    bool Menu::HandleChar(const unsigned int c)
    {
        return false;
    }

    bool Menu::HandleCursorPos(const double& xpos, const double& ypos)
    {
        m_mousePos.Set(xpos, ypos);
        return false;
    }

    bool Menu::HandleMouseButton(const int button, const int action, const int mods)
    {
        return false;
    }

    bool Menu::HandleWindowSize(const int width, const int height)
    {
        double pixelSize = 2.0 / height;
        m_font.PixelSize(pixelSize);
        return false;
    }

    bool Menu::HandleScroll(const double x, const double y)
    {
        return false;
    }


}; // namespace Viewer

