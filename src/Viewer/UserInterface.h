#ifndef USERINTERFACE_H
#define USERINTERFACE_H 1

namespace Viewer
{
    class UserInterface
    {
    public:
        UserInterface();
        ~UserInterface();

        bool Init();
        void Run();
        bool Cleanup();
        void Draw();

    };
}

#endif // USERINTERFACE_H

