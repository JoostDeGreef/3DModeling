#ifndef USERINTERFACE_H
#define USERINTERFACE_H 1

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

    private:
        std::shared_ptr<Internal::State> m_state;
    };
}

#endif // USERINTERFACE_H

