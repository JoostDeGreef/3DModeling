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
        void Exit();

        void AddShape(Geometry::ShapePtr& shape);
        void SetRenderMode(const Geometry::RenderMode renderMode);

        Menu& GetMenu();

    private:
        std::shared_ptr<Internal::State> m_state;
    };
}


