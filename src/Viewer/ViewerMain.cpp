#include <cstdlib>

#include "Geometry.h"
using namespace Geometry;

#include "Font.h"
#include "Menu.h"
#include "UserInterface.h"
using namespace Viewer;

void menu_exit(UserInterface& ui)
{
    ui.Exit();
}

void menu_settings_fps()
{
}

int main(int argc, char* argv[])
{
    UserInterface ui;

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


