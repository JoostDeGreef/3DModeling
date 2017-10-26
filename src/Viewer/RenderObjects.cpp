#include <functional>

using namespace std;

#include "Geometry.h"
#include "GLWrappers.h"
#include "RenderObjects.h"

using namespace Geometry;

namespace Viewer
{

    HullRenderObject::~HullRenderObject()
    {
        DisposeRenderObject(m_displayList);
    }

    void HullRenderObject::Check()
    {
        if (m_needsUpdate > 0)
        {
            m_needsUpdate = 0;
            glDeleteLists(m_displayList, 1);
            m_displayList = 0;
        }
    }

    HullRenderObject& GetRenderObject(const Geometry::HullRaw& hull)
    {
        HullRenderObject* res = dynamic_cast<HullRenderObject*>(hull->GetRenderObject());
        if (!res)
        {
            hull->SetRenderObject(std::make_unique<HullRenderObject>());
            res = dynamic_cast<HullRenderObject*>(hull->GetRenderObject());
            assert(res);
        }
        res->Check();
        return *res;
    }

    namespace
    {
        class GarbageMan
        {
        public:
            void Add(DisposedRenderObject &&disposedRenderObject)
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_disposedRenderObjects.emplace_back(std::move(disposedRenderObject));
            }
            void Clean()
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                for (DisposedRenderObject& disposedRenderObject : m_disposedRenderObjects)
                {
                    disposedRenderObject.Dispose();
                }
                m_disposedRenderObjects.clear();
            }
        private:
            std::vector<DisposedRenderObject> m_disposedRenderObjects;
            std::mutex m_mutex;
        };
        GarbageMan& GetGarbageMan()
        {
            static GarbageMan gm;
            return gm;
        }
    }

    void DisposeRenderObject(DisposedRenderObject &&disposedRenderObject)
    {
        GetGarbageMan().Add(std::move(disposedRenderObject));
    }
    void HandleDisposedRenderObjects()
    {
        GetGarbageMan().Clean();
    }

    void DisposedRenderObject::Dispose()
    {
        glDeleteLists(m_displayList, 1);
        m_displayList = 0;
    }


    // todo: 
    // - separate file
    // - store old values for pop to work.
    // - Getting objects properties is racy!!

    RenderInfo::RenderInfo()
    {
        static Color color(1, 1, 1, 1);
        glColor(color);
        m_color = &color;
    }

    void RenderInfo::Push(const Geometry::ShapeRaw & shape)
    {
        m_stack.emplace([]() {});
    }

    void RenderInfo::Push(const Geometry::HullRaw & hull)
    {
        const ColorPtr & color = hull->GetColor();
        if (color)
        {
            m_stack.emplace([]() {});
            glColor(color);
        }
        else
        {
            m_stack.emplace([]() {});
        }
    }

    void RenderInfo::Push(const Geometry::FaceRaw & face)
    {
        m_stack.emplace([]() {});
    }

    void RenderInfo::Push(const Geometry::EdgeRaw & edge)
    {
        m_stack.emplace([]() {});
    }

    void RenderInfo::Pop()
    {
        m_stack.top()();
        m_stack.pop();
    }

} // Viewer
