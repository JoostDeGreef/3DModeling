#include <cstdlib>
#include <memory>
using namespace std;

#include "Geometry.h"
using namespace Geometry;

#include "Font.h"
#include "Menu.h"
#include "UserInterface.h"
#include "Settings.h"
#include "Filesystem.h"
using namespace Viewer;

class FPSMenuItem : public MenuItem
{
public:
    FPSMenuItem(Menu& menu)
        : MenuItem(menu)
    {
        m_showFPS = Settings::GetBool("ShowFPS");
    }

    virtual const std::string GetText() const override
    {
        return m_showFPS ? "FPS On" : "FPS Off";
    }

    virtual void Execute() override
    {
        m_showFPS = !m_showFPS;
        Settings::SetBool("ShowFPS", m_showFPS);
    }
protected:
    bool m_showFPS;
};

class RenderModeMenuItem : public StaticMenuItem
{
    class RenderModeMenuSubItem : public StaticMenuItem
    {
    public:
        RenderModeMenuSubItem(RenderModeMenuItem& parent, std::string&& text, RenderMode renderMode)
            : StaticMenuItem(parent.GetMenu(), std::move(text))
            , m_parent(parent)
            , m_renderMode(renderMode)
        {}
        void Execute() override
        {
            m_parent.SetRenderMode(m_renderMode);
        }
        void Draw(const double& x, const double& y, const double& pixel, const bool mouseOver) override
        {
            auto color = mouseOver ? Geometry::Color::Red() 
                                   : (m_parent.GetRenderMode() == m_renderMode ? Geometry::Color::Green()
                                                                               : Geometry::Color::White());
            m_parent.GetMenu().GetFont().PixelSize(pixel).Color(color).Draw(x, y, GetText());
        }

    private:
        RenderModeMenuItem &m_parent;
        RenderMode m_renderMode;
    };
public:
    RenderModeMenuItem(Menu& menu, UserInterface& ui)
        : StaticMenuItem(menu, "RenderMode")
        , m_ui(ui)
    {
        using namespace std::placeholders;

        m_renderMode = (RenderMode)Geometry::Numerics::Clamp(Settings::GetInt("RenderMode",0),0,2);

        Add(make_shared<RenderModeMenuSubItem>(*this, "Solid", RenderMode::Solid));
        Add(make_shared<RenderModeMenuSubItem>(*this, "WireFrame", RenderMode::WireFrame));
        Add(make_shared<RenderModeMenuSubItem>(*this, "Transparent", RenderMode::Transparent));
    }

    RenderMode GetRenderMode() const
    {
        return m_renderMode;
    }
    void SetRenderMode(RenderMode renderMode)
    {
        m_renderMode = renderMode;
        m_ui.SetRenderMode(GetRenderMode());
        Settings::SetInt("RenderMode", (int)m_renderMode);
    }
    Menu& GetMenu() { return m_menu; }
protected:
    RenderMode m_renderMode;
    UserInterface& m_ui;

};

void menu_exit(MenuItem& menuItem, UserInterface& ui)
{
    ui.Exit();
}

int main(int argc, char* argv[])
{
    using namespace std::placeholders;

    // initialize settings
    std::string settingsDb = Filesystem::GetSettingsFilepath();
    Settings::AttachDB(settingsDb);

    // initialize UI
    UserInterface ui;
    if (!ui.Init())
    {
        return EXIT_FAILURE;
    }

    // create menu
    Menu& menu = ui.GetMenu();
    auto& settings = menu.Add(make_shared<StaticMenuItem>(menu, "Settings"));
    settings->Add(make_shared<FPSMenuItem>(menu));
    auto& renderMode = settings->Add(make_shared<RenderModeMenuItem>(menu, ui));
    menu.Add(make_shared<StaticCommandMenuItem>(menu, "Exit", std::bind(menu_exit, _1, ui)));

    // add something to draw
    ShapePtr s = Construct<Dodecahedron>(12);
    s->SetColor(Construct<Color>(1.0f, 1.0f, 1.0f, 1.0f));
    ui.AddShape(s);

    // apply initial settings
    ui.SetRenderMode(dynamic_pointer_cast<RenderModeMenuItem>(renderMode)->GetRenderMode());

    // main loop
    ui.Run();
    ui.Cleanup();

    return 0;
}


