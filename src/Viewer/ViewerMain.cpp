#include <cstdlib>
#include <memory>
using namespace std;

#include "Geometry.h"
using namespace Geometry;

#include "Font.h"
#include "Menu.h"
#include "UserInterface.h"
#include "Settings.h"
using namespace Viewer;

std::string GetUserPath() 
{
    char const* home = getenv("HOME");
    if (home || (home = getenv("USERPROFILE")))
    {
        return home;
    }
    else 
    {
        char const *hdrive = getenv("HOMEDRIVE");
        char const *hpath = getenv("HOMEPATH");
        return std::string(hdrive) + hpath;
    }
}

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
public:
    RenderModeMenuItem(Menu& menu, UserInterface& ui)
        : StaticMenuItem(menu, "RenderMode")
        , m_ui(ui)
    {
        using namespace std::placeholders;

        m_renderMode = Settings::GetInt("RenderMode",0);

        Add(make_shared<StaticCommandMenuItem>(menu, "Solid", std::bind(&RenderModeMenuItem::SetRenderMode, *this, _1, 0)));
        Add(make_shared<StaticCommandMenuItem>(menu, "WireFrame", std::bind(&RenderModeMenuItem::SetRenderMode, *this, _1, 1)));
        Add(make_shared<StaticCommandMenuItem>(menu, "Transparent", std::bind(&RenderModeMenuItem::SetRenderMode, *this, _1, 2)));
    }

    RenderMode GetRenderMode() const
    {
        switch (m_renderMode)
        {
        default:
            assert(false);
        case 0: return RenderMode::Solid;
        case 1: return RenderMode::WireFrame;
        case 2: return RenderMode::Transparent;
        }
    }
protected:
    int m_renderMode;
    UserInterface& m_ui;

private:
    void SetRenderMode(MenuItem&, int renderMode)
    {
        m_renderMode = renderMode;
        m_ui.SetRenderMode(GetRenderMode());
    }
};

void menu_exit(MenuItem& menuItem, UserInterface& ui)
{
    ui.Exit();
}

int main(int argc, char* argv[])
{
    using namespace std::placeholders;

    // initialize settings
    std::string settingsDb = GetUserPath() + "/3DModelingViewer.settings";
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


