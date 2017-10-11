#include <cstdlib>

#include "Dodecahedron.h"
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

    ShapePtr s = Dodecahedron::Construct(12);
    ui.AddShape(s);

    ui.Run();
    ui.Cleanup();

    return 0;
}


