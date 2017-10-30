#pragma once

namespace Viewer
{
    class RenderInfo
    {
    public:
        RenderInfo();
        void Push(const Geometry::ShapeRaw& shape);
        void Push(const Geometry::HullRaw& hull);
        void Push(const Geometry::FaceRaw& face);
        void Push(const Geometry::EdgeRaw& edge);
        void Pop();
    private:
        void Update(const Geometry::Color* color);
        void Update(const Geometry::RenderMode renderMode);

        struct State
        {
            const Geometry::Color* m_color;
            Geometry::RenderMode m_renderMode;
        } m_state;

        std::stack<State> m_stack;
    };

}; // Viewer


