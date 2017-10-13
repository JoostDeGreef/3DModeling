#include <cstdlib>

#include "Geometry.h"
using namespace Geometry;

#include "UserInterface.h"
using namespace Viewer;

int main(int argc, char* argv[])
{
    UserInterface ui;

    if (!ui.Init())
    {
        return EXIT_FAILURE;
    }

    ShapePtr s = Construct<Dodecahedron>(12);
    ui.AddShape(s);

    ui.Run();
    ui.Cleanup();

    return 0;
}


