#pragma once

namespace Viewer
{
    enum class MenuState
    {
        None,
        Selected,
        Opened,
    };

    class Menu;

    class MenuItem
    {
    public:
        MenuItem(Menu& menu)
            : m_items()
            , m_state(MenuState::None)
            , m_menu(menu)
        {}

        // methods used for dynamic menus
        virtual const std::string GetText() const { return ""; }
        virtual void Execute() {}

        // every menu can contain a submenu
        const std::shared_ptr<MenuItem>& Add(std::shared_ptr<MenuItem>&& menuItem) 
        { 
            m_items.emplace_back(std::move(menuItem)); 
            return m_items.back(); 
        }
        const std::shared_ptr<MenuItem>& operator [] (const int index) const
        { 
            return m_items.at(index); 
        }
        size_t Size() const
        { 
            return m_items.size(); 
        }

        const Geometry::BoundingShape2d& GetBBox() const
        {
            return m_bboxText;
        }
        const MenuState GetState() const
        {
            return m_state;
        }
        void SetState(const MenuState state)
        {
            for (auto& item : m_items)
            {
                item->m_state = MenuState::None;
            }
            m_state = state;
        }

    protected:
        Menu & m_menu; // main menu
        std::vector<std::shared_ptr<MenuItem>> m_items; // sub menu items, if any
        MenuState m_state;
        Geometry::BoundingShape2d m_bboxText;

        void DrawItems(double x, double y, double pixel);


    };

    class StaticMenuItem : public MenuItem
    {
    public:
        StaticMenuItem(Menu& menu, std::string&& text)
            : MenuItem(menu)
            , m_text(std::move(text))
        {}

        virtual const std::string GetText() const override
        { 
            return m_text; 
        }

    protected:
        std::string m_text;

    };

    class StaticCommandMenuItem : public StaticMenuItem
    {
    public:
        StaticCommandMenuItem(Menu& menu, std::string&& text, std::function<void(StaticCommandMenuItem& menuItem)>&& command)
            : StaticMenuItem(menu,std::move(text))
            , m_command(std::move(command))
        {}

        virtual void Execute() override
        {
            if (m_command)
            {
                m_command(*this);
            }
        }

    protected:
        std::function<void(StaticCommandMenuItem& menuItem)> m_command;
    };

    class Menu : public MenuItem
    {
        friend class MenuItem;
    public:
        Menu(const Font& font);

        // Draw the menu. 
        void Draw(int width,int height,double x,double y);

        // returns true if the event is handled by the menu
        bool HandleKey(const int key, const int scancode, const int action, const int mods);
        bool HandleChar(const unsigned int c);
        bool HandleCursorPos(const double& xpos, const double& ypos);
        bool HandleMouseButton(const int button, const int action, const int mods);
        bool HandleWindowSize(const int width, const int height);
        bool HandleScroll(const double x, const double y);

        const Geometry::Vector2d& GetMousePos() const
        {
            return m_mousePos;
        }

    protected:
        Font m_font;
        Geometry::Vector2d m_rawMousePos;
        Geometry::Vector2d m_mousePos;

        void DrawHamburger(double x, double y, double pixel);
    };
} // namespace Viewer
