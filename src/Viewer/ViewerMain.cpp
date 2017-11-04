#include <cstdlib>

#include "Geometry.h"
using namespace Geometry;

#include "Menu.h"
#include "UserInterface.h"
using namespace Viewer;

void menu_exit()
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
    menu.Add(MenuItem("Exit",menu_exit));

    ShapePtr s = Construct<Dodecahedron>(12);
    s->SetColor(Construct<Color>(1.0f,1.0f,1.0f,1.0f));
    ui.AddShape(s);

    ui.Run();
    ui.Cleanup();

    return 0;
}


