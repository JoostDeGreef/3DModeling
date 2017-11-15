#pragma once

namespace Viewer
{
    enum class MenuState
    {
        None,
        Selected,
        Opened,
    };

    class MenuItem
    {
    public:
        MenuItem() 
            : m_items()
            , m_text() 
            , m_command([]() {})
            , m_state(MenuState::None)
        {}
        MenuItem(MenuItem&& other) 
            : m_items(std::move(other.m_items))
            , m_text(std::move(other.m_text)) 
            , m_command(std::move(other.m_command))
            , m_state(MenuState::None)
        {}
        MenuItem(const MenuItem& other) 
            : m_items(other.m_items)
            , m_text(other.m_text) 
            , m_command(other.m_command)
            , m_state(MenuState::None)
        {}
        MenuItem(char * text) 
            : m_items() 
            , m_text(text) 
            , m_command([]() {})
            , m_state(MenuState::None)
        {}
        MenuItem(char * text,std::function<void()>&& command)
            : m_items()
            , m_text(text)
            , m_command(std::move(command))
            , m_state(MenuState::None)
        {}
        MenuItem(std::string&& text)
            : m_items()
            , m_text(std::move(text))
            , m_command([]() {})
            , m_state(MenuState::None)
        {}
        MenuItem(std::string&& text, std::function<void()>&& command)
            : m_items()
            , m_text(std::move(text)) 
            , m_command(std::move(command))
            , m_state(MenuState::None)
        {}

        MenuItem& Add(MenuItem&& menuItem) 
        { 
            m_items.emplace_back(std::move(menuItem)); 
            return m_items.back(); 
        }
        MenuItem& operator [] (const int index) 
        { 
            return m_items.at(index); 
        }
        size_t Size() 
        { 
            return m_items.size(); 
        }

    protected:
        std::vector<MenuItem> m_items;
        std::string m_text;
        std::function<void()> m_command;
        MenuState m_state;
        Geometry::BoundingShape2d m_bbox;
    };

    class Menu : public MenuItem
    {
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

    private:
        int m_prevWidth;
        int m_prevHeight;
        int m_displayList;
        double m_prevOpacity;

        Font m_font;
        Geometry::BoundingShape2d m_bboxHamburger;
        Geometry::Vector2d m_mousePos;

        void DrawHamburger(int width, int height, double X, double Y);
        void DrawExpanded(int width, int height, double X, double Y);
    };
} // namespace Viewer
