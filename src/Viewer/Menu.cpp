#include "Geometry.h"
#include "Font.h"
#include "Menu.h"
#include "GLWrappers.h"

namespace Viewer
{
    Menu::Menu()
        : MenuItem(*this)
        , m_font()
        , m_border(5)
    {}

    void Menu::Draw(int width, int height, double X, double Y)
    {
        // size of one pixel
        double pixel = 2.0 / height;

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
    Geometry::Vector2d MenuItem::GetSize()
    {
        return m_menu.m_font->GetSize(GetText());
    }
    Geometry::Vector2d DelimiterMenuItem::GetSize()
    {
        return Geometry::Vector2d(-1, -1);
    }
    void MenuItem::DrawItems(double x, double y, double pixel)
    {
        const int bw = m_menu.m_border; // borderwidth
        const int ii = 2; // gap between items
        const int ib = 8; // gap below items

        Geometry::Vector2d total(0,0);
        std::vector<Geometry::Vector2d> itemSizes;
        for (const auto& item : m_items)
        {
            Geometry::Vector2d size = item->GetSize();
            if (size[0] < 0)
            {
                size[1] = (ii + bw + ib)*pixel;
            }
            total[0] = std::max(total[0], size[0]);
            total[1] += size[1];
            itemSizes.emplace_back(size);
        }

        total[0] += 2 * ii * pixel;
        total[1] += ((m_items.size() - 1)*ii + ib)*pixel;

        Geometry::BoundingShape2d box(Geometry::Vector2d(x, y),Geometry::Vector2d(x + bw * 2 * pixel + total[0], y - bw * 2 * pixel - total[1]));

        // menu opacity changes on mousehover
        double opacity = 0.5;
        if (!m_menu.m_selected && box.Encapsulates(m_menu.m_mousePos))
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
            glColor4d(0, 0, 0, 0.6);
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
            if (itemSizes[i][0] >= 0)
            {
                if (m_items[i]->Size())
                {
                    if (m_items[i]->GetState() == MenuState::Opened)
                    {
                        glColor4d(1, 1, 1, 1);
                    }
                    else
                    {
                        glColor4d(1, 1, 1, 0.5);
                    }
                    glBegin(GL_TRIANGLES);
                        // block
                        glVertex2d(x + total[0] + 1 * bw*pixel, y + itemSizes[i][1] / 2 + 2 * bw*pixel);
                        glVertex2d(x + total[0] + 1 * bw*pixel, y + itemSizes[i][1] / 2 - 2 * bw*pixel);
                        glVertex2d(x + total[0] + 3 * bw*pixel, y + itemSizes[i][1] / 2);
                        glEnd();
                }
                m_items[i]->m_bboxText.Set(Geometry::Vector2d(x + ii*pixel, y), Geometry::Vector2d(x + itemSizes[i][0], y + itemSizes[i][1]));
                m_items[i]->Draw(x + ii*pixel, y, pixel, m_items[i].get() == m_menu.m_selected || (!m_menu.m_selected && m_items[i]->m_bboxText.Encapsulates(m_menu.m_mousePos)));
            }
            else
            {
                glBegin(GL_QUADS);
                    // line
                    glColor4d(1, 1, 1, opacity);
                    glVertex2d(x, y - (   - ii) * pixel);
                    glVertex2d(x, y - (bw - ii) * pixel);
                    glVertex2d(x + total[0] - 0 * bw*pixel, y - (bw - ii) * pixel);
                    glVertex2d(x + total[0] - 0 * bw*pixel, y - (   - ii) * pixel);
                glEnd();
            }
            if (m_items[i]->GetState() == MenuState::Opened)
            {
                m_items[i]->DrawItems(x+bw*2*pixel+ total[0],y + itemSizes[i][1] + ii*pixel,pixel);
            }
        }
    }
    void MenuItem::Draw(const double& x, const double& y, const double& pixel, const bool mouseOver)
    {
        m_menu.m_font->PixelSize(pixel).Color(mouseOver ? Geometry::Color::Red() : Geometry::Color::White()).Draw(x, y, GetText());
    }
    void MenuItem::SetState(const MenuState state)
    {
        for (auto& item : m_items)
        {
            item->m_state = MenuState::None;
        }
        if (m_parent)
        {
            for (auto& item : m_parent->m_items)
            {
                item->m_state = MenuState::None;
            }
        }
        m_state = state;
        m_menu.Select(nullptr);
    }

    bool Menu::HandleKey(const int key, const int scancode, const int action, const int mods)
    {
        // esc: key = GLFW_KEY_ESCAPE, scancode = 1, action = GLFW_PRESS, mods = 0
        if (m_state == MenuState::None)
        {
            if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            {
                SetState(MenuState::Opened);
                m_menu.Select( !m_items.empty() ? m_items.front().get() : nullptr );
            }
        }
        else
        {
            std::function<MenuItem*(MenuItem*, const bool)> FindOpenMenu = [&](MenuItem* menuItem,const bool first)
            {
                size_t count = menuItem->Size();
                for (size_t i = 0; i < count; ++i)
                {
                    if ((*menuItem)[i]->GetState() == MenuState::Opened)
                    {
                        return FindOpenMenu((*menuItem)[i].get(),first);
                    }
                }
                if (count > 0)
                {
                    return (*menuItem)[first ? 0 : count-1].get();
                }
                return menuItem;
            };
            if (action == GLFW_PRESS)
            {
                if (!m_selected)
                {
                    // TODO: select item from mouse position
                }
                switch (key)
                {
                case GLFW_KEY_ESCAPE:
                    SetState(MenuState::None);
                    break;
                case GLFW_KEY_ENTER:
                    if (m_selected)
                    {
                        auto selected = m_selected;
                        if (selected->Size() > 0)
                        {
                            if (selected->GetState() == MenuState::Opened)
                            {
                                selected->SetState(MenuState::None);
                                m_menu.Select(selected);
                            }
                            else
                            {
                                selected->SetState(MenuState::Opened);
                                m_menu.Select((*selected)[0].get());
                            }
                        }
                        else
                        {
                            selected->Execute();
                            m_menu.SetState(MenuState::None);
                        }
                    }
                    break;
                case GLFW_KEY_DOWN:
                    if (!m_selected)
                    {
                        // find last entry in open menu
                        m_selected = FindOpenMenu(&m_menu, false);
                    }
                    if (m_selected)
                    {
                        m_selected->SelectNext();
                    }
                    break;
                case GLFW_KEY_UP:
                    if (!m_selected)
                    {
                        // find first entry in open menu
                        m_selected = FindOpenMenu(&m_menu, true);
                    }
                    if (m_selected)
                    {
                        m_selected->SelectPrev();
                    }
                    break;
                case GLFW_KEY_LEFT:
                    if (!m_selected)
                    {
                        // find first entry in open menu
                        m_selected = FindOpenMenu(&m_menu, true);
                    }
                    if (m_selected)
                    {
                        auto selected = m_selected->GetParent();
                        if (selected)
                        {
                            selected->SetState(MenuState::None);
                        }
                        m_selected = selected;
                    }
                    break;
                case GLFW_KEY_RIGHT:
                    if (m_selected && m_selected->Size())
                    {
                        m_selected->SetState(MenuState::Opened);
                    }
                    m_selected = FindOpenMenu(&m_menu, true);
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
        m_selected = nullptr;
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
                    std::function<bool(MenuItem*)> handleItem = [&](MenuItem* item)
                    {
                        bool res = false;
                        for (unsigned int i = 0; i<item->Size() && !res; ++i)
                        {
                            if ((*item)[i]->GetBBox().Encapsulates(m_menu.GetMousePos()))
                            {
                                if ((*item)[i]->Size() > 0)
                                {
                                    if ((*item)[i]->GetState() == MenuState::Opened)
                                    {
                                        (*item)[i]->SetState(MenuState::None);
                                    }
                                    else
                                    {
                                        (*item)[i]->SetState(MenuState::Opened);
                                    }
                                }
                                else
                                {
                                    (*item)[i]->Execute();
                                    m_menu.SetState(MenuState::None);
                                }
                                res = true;
                            }
                            else
                            {
                                if ((*item)[i]->GetState() == MenuState::Opened)
                                {
                                    res = handleItem((*item)[i].get());
                                }
                            }
                        }
                        return res;
                    };
                    res = handleItem(this);
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
        return false;
    }

    bool Menu::HandleScroll(const double x, const double y)
    {
        return false;
    }

    int MenuItem::FindIndex()
    {
        if (m_parent)
        {
            for (size_t i = 0; i < m_parent->Size(); ++i)
            {
                if ((*m_parent)[i].get() == this)
                {
                    return i;
                }
            }
        }
        return -1;
    }

    void MenuItem::SelectNext()
    {
        int index = FindIndex();
        if (index < 0)
        {
            assert(false); // broken menu structure
            m_menu.Select(m_parent);
        }
        else
        {
            index++;
            if (index >= m_parent->Size())
            {
                index = 0;
            }
            m_menu.Select((*m_parent)[index].get());
        }
    }

    void MenuItem::SelectPrev()
    {
        int index = FindIndex();
        if (index < 0)
        {
            assert(false); // broken menu structure
            m_menu.Select(m_parent);
        }
        else
        {
            index--;
            if (index < 0)
            {
                index = m_parent->Size()-1;
            }
            m_menu.Select((*m_parent)[index].get());
        }
    }

}; // namespace Viewer

