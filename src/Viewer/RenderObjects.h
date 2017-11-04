#pragma once

namespace Viewer
{
    class HullRenderObject : public Geometry::IRenderObject
    {
    public:
        HullRenderObject()
            : m_needsUpdate(42)
            , m_displayList(0)
        {}
        virtual ~HullRenderObject();
        virtual void Invalidate() override { ++m_needsUpdate; }
        unsigned int NeedsUpdate() { return m_needsUpdate; }

        unsigned int GetDisplayList() { return m_displayList; }
        void SetDisplayList(unsigned int displayList);

    private:
        volatile unsigned int m_needsUpdate;
        unsigned int m_displayList;
    };

    void DisposeAllRenderObjects();
    void HandleDisposedRenderObjects();

    HullRenderObject& GetRenderObject(const Geometry::HullRaw& hull);
}; // Viewer


