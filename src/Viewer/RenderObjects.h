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

    class PatchRenderObject : public Geometry::IRenderObject
    {
    public:
        PatchRenderObject()
            : m_needsUpdate(0)
            , m_displayList(0)
        {}
        virtual ~PatchRenderObject();
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

    PatchRenderObject& GetRenderObject(const Geometry::PatchRaw& patch);

}; // Viewer

#endif // VIEWER_RENDEROBJECTS_H

