#ifndef VIEWER_RENDEROBJECTS_H
#define VIEWER_RENDEROBJECTS_H 1

namespace Viewer
{
    class DisposedRenderObject
    {
    public:
        DisposedRenderObject(unsigned int displayList)
            : m_displayList(displayList)
        {}

        void Dispose();
    private:
        unsigned int m_displayList;
    };

    class HullRenderObject : public Geometry::IRenderObject
    {
    public:
        HullRenderObject()
            : m_needsUpdate(0)
            , m_displayList(0)
        {}
        virtual ~HullRenderObject();
        virtual void Invalidate() override 
        {
            ++m_needsUpdate;
        }

        unsigned int GetDisplayList()
        {
            return m_displayList;
        }

        void SetDisplayList(unsigned int displayList)
        {
            m_displayList = displayList;
        }

        void Check();
    private:
        unsigned int m_needsUpdate;
        unsigned int m_displayList;
    };

    void DisposeRenderObject(DisposedRenderObject &&disposedRenderObject);
    void HandleDisposedRenderObjects();

    HullRenderObject& GetRenderObject(const Geometry::HullRaw& hull);

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
        std::stack<std::function<void()>> m_stack;

        Geometry::Color* m_color;
    };

}; // Viewer

#endif // VIEWER_RENDEROBJECTS_H

