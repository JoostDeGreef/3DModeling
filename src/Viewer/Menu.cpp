#include "Geometry.h"
#include "Menu.h"
#include "GLWrappers.h"

namespace Viewer
{
    Menu::Menu()
        : m_prevWidth(0)
        , m_prevHeight(0)
        , m_displayList(0)
    {}

    void Menu::Draw(int width, int height, double X, double Y)
    {
        // menu opacity changes on mousehover
        double opacity = 0.5;

        glColor4d(1, 1, 1, opacity);

        if (width != m_prevWidth ||
            height != m_prevHeight)
        {
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
        }
        glCallList(m_displayList);
    }

    bool Menu::HandleKey(const int key, const int scancode, const int action, const int mods)
    {
        return false;
    }

    bool Menu::HandleChar(const unsigned int c)
    {
        return false;
    }

    bool Menu::HandleCursorPos(const double& xpos, const double& ypos)
    {
        return false;
    }

    bool Menu::HandleMouseButton(const int button, const int action, const int mods)
    {
        return false;
    }

    bool Menu::HandleWindowSize(const int width, const int height)
    {
        return false;
    }

    bool Menu::HandleScroll(const double x, const double y)
    {
        return false;
    }


}; // namespace Viewer

