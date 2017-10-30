#pragma once

namespace Viewer
{
    class Menu
    {
    public:
        Menu();

        // Draw the menu. 
        void Draw();

        // returns tru if the mouse event is handled by the menu
        bool HandleMouse(const int button, const int action, const int mods);

    };
} // namespace Viewer
