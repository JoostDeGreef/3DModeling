#pragma once

namespace Viewer
{
    class HullRenderObject : public Geometry::IRenderObject
    {
    public:
        HullRenderObject()
            : m_needsUpdate(42)
            , m_objectDefunct(false)
            , m_displayList(0)
        {}
        virtual ~HullRenderObject();
        virtual void Invalidate() override { ++m_needsUpdate; }
        virtual void ForcedInvalidate() override { m_objectDefunct = true; };

        unsigned int NeedsUpdate() { return m_needsUpdate; }
        bool ObjectDefunct() { return m_objectDefunct; }

        unsigned int GetDisplayList() { return m_displayList; }
        void SetDisplayList(unsigned int displayList);

    private:
        volatile unsigned int m_needsUpdate;
        volatile bool m_objectDefunct;
        unsigned int m_displayList;
    };

    void DisposeAllRenderObjects();
    void HandleDisposedRenderObjects();

    HullRenderObject& GetRenderObject(const Geometry::HullRaw& hull);
}; // Viewer


