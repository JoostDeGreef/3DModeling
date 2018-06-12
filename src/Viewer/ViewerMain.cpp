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
#include "Data.h"
using namespace Viewer;

#include "Menu/FPSMenuItem.h"
#include "Menu/RenderModeMenuItem.h"
#include "Menu/FontMenuItem.h"

void menu_exit(MenuItem& menuItem, UserInterface& ui)
{
    ui.Exit();
}

void menu_testcase_0(MenuItem& menuItem, UserInterface& ui)
{
    ui.ClearShapes();
    auto s = Construct<Dodecahedron>(12);
    s->SetColor(Construct<Color>(1.0f, 1.0f, 1.0f, 1.0f));
    ui.AddShape(s);
    ui.SetRenderMode(Settings::GetRenderMode());
}

void menu_testcase_1(MenuItem& menuItem, UserInterface& ui)
{
    ui.ClearShapes();
    auto s = Construct<Dodecahedron>(60);
    s->SetColor(Construct<Color>(1.0f, 1.0f, 1.0f, 1.0f));
    ui.AddShape(s);
    ui.SetRenderMode(Settings::GetRenderMode());
}

void menu_testcase_2(MenuItem& menuItem, UserInterface& ui)
{
    ui.ClearShapes();
    auto s = Construct<Cube>();
    s->Scale(1/sqrt(3));
    s->SetColor(Construct<Color>(1.0f, 1.0f, 1.0f, 1.0f));
    Geometry::ShapePtr t = Construct<Cube>();
    t->Scale(1 / sqrt(3));
    t->Translate({ 0.3,0.3,0.3 });
    t->SetColor(Construct<Color>(1.0f, 0.0f, 0.0f, 1.0f));
    s->Add(t);
    ui.AddShape(s);
    ui.SetRenderMode(Settings::GetRenderMode());
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

    auto settings = menu.Add(make_shared<StaticMenuItem>(menu, "Settings"));
    settings->Add(make_shared<FPSMenuItem>(menu));
    auto renderMode = settings->Add(make_shared<RenderModeMenuItem>(menu, ui));
    auto font = settings->Add(make_shared<FontMenuItem>(menu, ui));   
    auto testcases = menu.Add(make_shared<StaticMenuItem>(menu, "Testcases"));
    testcases->Add(make_shared<StaticCommandMenuItem>(menu, "Test case 0", std::bind(menu_testcase_0, _1, ui)));
    testcases->Add(make_shared<StaticCommandMenuItem>(menu, "Test case 1", std::bind(menu_testcase_1, _1, ui)));
    testcases->Add(make_shared<StaticCommandMenuItem>(menu, "Test case 2", std::bind(menu_testcase_2, _1, ui)));
    menu.Add(make_shared<StaticCommandMenuItem>(menu, "Exit", std::bind(menu_exit, _1, ui)));

    // apply initial settings
    ui.SetRenderMode(dynamic_pointer_cast<RenderModeMenuItem>(renderMode)->GetRenderMode());
    ui.SetViewingMode(dynamic_pointer_cast<RenderModeMenuItem>(renderMode)->GetViewingMode());
    ui.SetFontName(dynamic_pointer_cast<FontMenuItem>(font)->GetFontName());

    menu_testcase_1(*(testcases->operator[](1)), ui);

    // main loop
    ui.Run();
    ui.Cleanup();

    return 0;
}


