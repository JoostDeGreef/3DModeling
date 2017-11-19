#pragma once

namespace Viewer
{
    namespace Internal
    {
        class State;
    };

    class UserInterface
    {
    public:
        UserInterface();
        ~UserInterface();

        bool Init();
        void Run();
        bool Cleanup();
        void Draw();
        void AddShape(Geometry::ShapePtr& shape);
        void Exit();

        Menu& GetMenu();

    private:
        std::shared_ptr<Internal::State> m_state;
    };
}


