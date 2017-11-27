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

        void AddShape(const Geometry::ShapePtr& shape);
        void ClearShapes();

        void SetRenderMode(const Geometry::RenderMode renderMode);
        void SetFontName(const std::string& fontName);

        Menu& GetMenu();

    private:
        std::shared_ptr<Internal::State> m_state;
    };
}


