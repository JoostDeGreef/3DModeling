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

class RenderModeMenuItem : public MenuItem
{
public:
    RenderModeMenuItem(Menu& menu, UserInterface& ui)
        : MenuItem(menu)
        , m_ui(ui)
    {
        SetRenderMode(Settings::GetInt("RenderMode",0));
    }

    virtual const std::string GetText() const override
    {
        switch (m_renderMode)
        {
        default:
            assert(false);
        case 0: return "Solid";
        case 1: return "Wireframe";
        case 2: return "Transparent";
        }
    }

    virtual void Execute() override
    {
        SetRenderMode(m_renderMode + 1);
        Settings::SetInt("RenderMode", m_renderMode);
    }
protected:
    int m_renderMode;
    UserInterface& m_ui;

private:
    void SetRenderMode(int renderMode)
    {
        m_renderMode = renderMode;
        switch (m_renderMode)
        {
        default:
            m_renderMode = 0;
        case 0:
            m_ui.SetRenderMode(RenderMode::Solid);
            break;
        case 1:
            m_ui.SetRenderMode(RenderMode::WireFrame);
            break;
        // not supported yet
        //case 2:
        //    m_ui.SetRenderMode(RenderMode::Transparent);
        //    break;
        }
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

    // add something to draw
    ShapePtr s = Construct<Dodecahedron>(12);
    s->SetColor(Construct<Color>(1.0f, 1.0f, 1.0f, 1.0f));
    ui.AddShape(s);

    // create menu
    Menu& menu = ui.GetMenu();
    auto& settings = menu.Add(make_shared<StaticMenuItem>(menu, "Settings"));
    settings->Add(make_shared<FPSMenuItem>(menu));
    settings->Add(make_shared<RenderModeMenuItem>(menu, ui));
    menu.Add(make_shared<StaticCommandMenuItem>(menu, "Exit", std::bind(menu_exit, _1, ui)));

    // main loop
    ui.Run();
    ui.Cleanup();

    return 0;
}


