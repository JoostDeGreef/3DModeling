#include <cstdlib>

#include "UserInterface.h"
using namespace Viewer;

int main(int argc, char* argv[])
{
    UserInterface ui;

    if (!ui.Init())
    {
        return EXIT_FAILURE;
    }
    ui.Run();
    ui.Cleanup();

    return 0;
}


