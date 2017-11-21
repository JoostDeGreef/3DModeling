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
            , m_text() 
            , m_textFunction()
            , m_command([]() {})
            , m_state(MenuState::None)
            , m_menu(menu)
        {}
        MenuItem(MenuItem&& other) 
            : m_items(std::move(other.m_items))
            , m_text(std::move(other.m_text)) 
            , m_textFunction()
            , m_command(std::move(other.m_command))
            , m_state(MenuState::None)
            , m_menu(other.m_menu)
        {}
        MenuItem(const MenuItem& other) 
            : m_items(other.m_items)
            , m_text(other.m_text) 
            , m_textFunction()
            , m_command(other.m_command)
            , m_state(MenuState::None)
            , m_menu(other.m_menu)
        {}
        MenuItem(Menu& menu,char * text)
            : m_items() 
            , m_text(text) 
            , m_textFunction()
            , m_command([]() {})
            , m_state(MenuState::None)
            , m_menu(menu)
        {}
        MenuItem(Menu& menu,char * text,std::function<void()>&& command)
            : m_items()
            , m_text(text)
            , m_textFunction()
            , m_command(std::move(command))
            , m_state(MenuState::None)
            , m_menu(menu)
        {}
        MenuItem(Menu& menu,std::string&& text)
            : m_items()
            , m_text(std::move(text))
            , m_textFunction()
            , m_command([]() {})
            , m_state(MenuState::None)
            , m_menu(menu)
        {}
        MenuItem(Menu& menu,std::string&& text, std::function<void()>&& command)
            : m_items()
            , m_text(std::move(text)) 
            , m_textFunction()
            , m_command(std::move(command))
            , m_state(MenuState::None)
            , m_menu(menu)
        {}
        MenuItem(Menu& menu, std::function<std::string()>&& textFunction, std::function<void()>&& command)
            : m_items()
            , m_text()
            , m_textFunction(std::move(textFunction))
            , m_command(std::move(command))
            , m_state(MenuState::None)
            , m_menu(menu)
        {}

        MenuItem& Add(MenuItem&& menuItem) 
        { 
            m_items.emplace_back(std::move(menuItem)); 
            return m_items.back(); 
        }
        const MenuItem& operator [] (const int index) const
        { 
            return m_items.at(index); 
        }
        MenuItem& operator [] (const int index)
        {
            return m_items.at(index);
        }
        size_t Size() const
        { 
            return m_items.size(); 
        }
        const std::string& GetText() const
        {
            if (m_textFunction)
            {
                std::string text = m_textFunction();
                return text;
            }
            return m_text;
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
            for (auto& item: m_items)
            {
                item.m_state = MenuState::None;
            }
            m_state = state;
        }
        void Execute() const
        {
            m_command();
        }
    protected:
        Menu & m_menu;
        std::vector<MenuItem> m_items;
        std::string m_text;
        std::function<std::string()> m_textFunction;
        std::function<void()> m_command;
        MenuState m_state;
        Geometry::BoundingShape2d m_bboxText;

        void DrawItems(double x, double y, double pixel);
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
