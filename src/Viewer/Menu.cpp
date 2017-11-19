#include "Geometry.h"
#include "Font.h"
#include "Menu.h"
#include "GLWrappers.h"

namespace Viewer
{
    Menu::Menu(const Font& font)
        : MenuItem(*this)
        , m_font(font)
    {}

    void Menu::Draw(int width, int height, double X, double Y)
    {
        // size of one pixel
        double pixel = X / width;

        // start position
        double x = -X + 5 * pixel;
        double y = Y - 5 * pixel;

        // mouse position in view coordinates
        m_mousePos.Set(m_rawMousePos[0] * 2.0*X / width - X, Y - m_rawMousePos[1] * 2.0*Y / height);

        if (m_state == MenuState::None)
        {
            DrawHamburger(x, y, pixel);
        }
        else
        {
            DrawItems(x, y, pixel);
        }
    }
    void Menu::DrawHamburger(double x, double y, double pixel)
    {

        m_font.Color(Geometry::Color::Red()).Draw(0, 0, std::to_string(m_mousePos[0]) + " x " + std::to_string(m_mousePos[1]));

        int bh = 15; // one bar heigth
        int gh = 8;  // gap height

        m_bboxText.Set(Geometry::Vector2d(x + 0 * bh*pixel, y - ((bh + gh) * 0 + 0)*pixel),
                       Geometry::Vector2d(x + (bh * 3 + gh * 2)*pixel, y - ((bh + gh) * 2 + bh)*pixel));

        // menu opacity changes on mousehover
        double opacity = 0.5;
        if (m_bboxText.Encapsulates(m_mousePos))
        {
            opacity = 1.0;
        }

        glColor4d(1, 1, 1, opacity);
        for (int i = 0; i < 3; ++i)
        {
            glBegin(GL_QUADS);
                glVertex2d(x + 0*bh*pixel, y - ((bh + gh)*i + 0)*pixel);
                glVertex2d(x + 0*bh*pixel, y - ((bh + gh)*i + bh)*pixel);
                glVertex2d(x + (bh*3 + gh*2)*pixel, y - ((bh + gh)*i + bh)*pixel);
                glVertex2d(x + (bh*3 + gh*2)*pixel, y - ((bh + gh)*i + 0)*pixel);
            glEnd();
        }
    }
    void MenuItem::DrawItems(double x, double y, double pixel)
    {
        Geometry::Vector2d total(0,0);
        std::vector<Geometry::Vector2d> itemSizes;
        for (const auto& item : m_items)
        {
            Geometry::Vector2d size = m_menu.m_font.GetSize(item.GetText());
            total[0] = std::max(total[0], size[0]);
            total[1] += size[1];
            itemSizes.emplace_back(size);
        }

        int bw = 5; // borderwidth
        int ii = 1; // gap between items
        int ib = 8; // gap below items
        total[1] += ((m_items.size() - 1)*ii + ib)*pixel;

        Geometry::BoundingShape2d box(Geometry::Vector2d(x, y),Geometry::Vector2d(x + bw * 2 * pixel + total[0], y - bw * 2 * pixel - total[1]));

        // menu opacity changes on mousehover
        double opacity = 0.5;
        if (box.Encapsulates(m_menu.m_mousePos))
        {
            opacity = 1.0;
        }

        glBegin(GL_QUADS);
            // borders
            glColor4d(1, 1, 1, opacity);
            // left
            glVertex2d(x + 0 * bw*pixel, y - 0 * bw * pixel );
            glVertex2d(x + 0 * bw*pixel, y - 2 * bw * pixel - total[1]);
            glVertex2d(x + 1 * bw*pixel, y - 1 * bw * pixel - total[1]);
            glVertex2d(x + 1 * bw*pixel, y - 1 * bw * pixel);
            // right
            glVertex2d(x + 2 * bw*pixel + total[0], y - 2 * bw * pixel - total[1]);
            glVertex2d(x + 2 * bw*pixel + total[0], y - 0 * bw * pixel);
            glVertex2d(x + 1 * bw*pixel + total[0], y - 1 * bw * pixel);
            glVertex2d(x + 1 * bw*pixel + total[0], y - 1 * bw * pixel - total[1]);
            // top
            glVertex2d(x + 1 * bw*pixel + total[0], y - 1 * bw * pixel);
            glVertex2d(x + 2 * bw*pixel + total[0], y - 0 * bw * pixel);
            glVertex2d(x + 0 * bw*pixel, y - 0 * bw * pixel);
            glVertex2d(x + 1 * bw*pixel, y - 1 * bw * pixel);
            // bottom
            glVertex2d(x + 2 * bw*pixel + total[0], y - 2 * bw * pixel - total[1]);
            glVertex2d(x + 1 * bw*pixel + total[0], y - 1 * bw * pixel - total[1]);
            glVertex2d(x + 1 * bw*pixel, y - 1 * bw * pixel - total[1]);
            glVertex2d(x + 0 * bw*pixel, y - 2 * bw * pixel - total[1]);
            // center
            glColor4d(1, 1, 1, 0.2);
            glVertex2d(x + 1 * bw*pixel, y - 1 * bw * pixel);
            glVertex2d(x + 1 * bw*pixel, y - 1 * bw * pixel - total[1]);
            glVertex2d(x + 1 * bw*pixel + total[0], y - 1 * bw * pixel - total[1]);
            glVertex2d(x + 1 * bw*pixel + total[0], y - 1 * bw * pixel);
        glEnd();

        x += bw*pixel;
        y -= bw*pixel;
        for (unsigned int i=0;i<m_items.size();++i)
        {
            y -= itemSizes[i][1] + ii*pixel;
            m_items[i].m_bboxText.Set(Geometry::Vector2d(x,y), Geometry::Vector2d(x + itemSizes[i][0], y + itemSizes[i][1]));
            m_menu.m_font.Color(m_items[i].m_bboxText.Encapsulates(m_menu.m_mousePos) ? Geometry::Color::Red() : Geometry::Color::White() ).Draw(x, y, m_items[i].GetText());
            if (m_items[i].GetState() == MenuState::Opened)
            {
                m_items[i].DrawItems(x+bw*2*pixel+ total[0],y + itemSizes[i][1] + ii*pixel,pixel);
            }
        }
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
        m_rawMousePos.Set(xpos, ypos);
        return false;
    }

    bool Menu::HandleMouseButton(const int button, const int action, const int mods)
    {
        bool res = false;

        if (GLFW_MOUSE_BUTTON_LEFT == button && 0 == mods)
        {
            if (GLFW_PRESS == action)
            {
                if (m_state == MenuState::None)
                {
                    if (m_bboxText.Encapsulates(m_menu.m_mousePos))
                    {
                        SetState(MenuState::Opened);
                        res = true;
                    }
                }
                else
                {
                    std::function<bool(MenuItem&)> handleItem = [&](MenuItem& item)
                    {
                        bool res = false;
                        for (unsigned int i = 0; i<item.Size() && !res; ++i)
                        {
                            if (item[i].GetBBox().Encapsulates(m_menu.GetMousePos()))
                            {
                                if (item[i].Size() > 0)
                                {
                                    if (item[i].GetState() == MenuState::Opened)
                                    {
                                        item[i].SetState(MenuState::None);
                                    }
                                    else
                                    {
                                        item[i].SetState(MenuState::Opened);
                                    }
                                }
                                else
                                {
                                    item[i].Execute();
                                    m_menu.SetState(MenuState::None);
                                }
                                res = true;
                            }
                            else
                            {
                                if (item[i].GetState() == MenuState::Opened)
                                {
                                    res = handleItem(item[i]);
                                }
                            }
                        }
                        return res;
                    };
                    res = handleItem(*this);
                    if (!res)
                    {
                        m_state = MenuState::None;
                    }
                }
            }
        }

        return res;
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

