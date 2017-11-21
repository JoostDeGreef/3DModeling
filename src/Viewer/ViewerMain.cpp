#include <cstdlib>
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

void menu_exit(UserInterface& ui)
{
    ui.Exit();
}

void menu_settings_fps()
{
    bool showFPS = Settings::GetBool("ShowFPS");
    Settings::SetBool("ShowFPS", !showFPS);
}

int main(int argc, char* argv[])
{
    UserInterface ui;

    std::string settingsDb = GetUserPath() + "/3DModelingViewer.settings";
    Settings::AttachDB(settingsDb);

    if (!ui.Init())
    {
        return EXIT_FAILURE;
    }

    Menu& menu = ui.GetMenu();
    auto& settings = menu.Add(MenuItem(menu, "Settings"));
    settings.Add(MenuItem(menu, "FPS", menu_settings_fps));
    menu.Add(MenuItem(menu, "Exit", std::bind(menu_exit, ui)));

    ShapePtr s = Construct<Dodecahedron>(12);
    s->SetColor(Construct<Color>(1.0f,1.0f,1.0f,1.0f));
    ui.AddShape(s);

    ui.Run();
    ui.Cleanup();

    return 0;
}


