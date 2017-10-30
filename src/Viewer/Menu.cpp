#include "Geometry.h"
#include "Menu.h"
#include "GLWrappers.h"

namespace Viewer
{
    Menu::Menu()
    {}

    void Menu::Draw()
    {
        double opacity = 0.5;

        double x = 0;
        double y = 0;
        double w = 0.2;
        double h = 0.2;

        glBegin(GL_QUADS);
        glColor4d(1, 0, 0, opacity);
        glVertex2d(x+0, y+0);
        glVertex2d(x+0, y-h);
        glVertex2d(x+w, y-h);
        glVertex2d(x+w, y+0);
        glEnd();
    }

    bool Menu::HandleMouse(const int button, const int action, const int mods)
    {
        return false;
    }

}; // namespace Viewer

