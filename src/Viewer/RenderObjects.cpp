#include "Geometry.h"
#include "GLWrappers.h"
#include "RenderObjects.h"

namespace Viewer
{

    PatchRenderObject::~PatchRenderObject()
    {
        DisposeRenderObject(m_displayList);
    }

    void PatchRenderObject::Check()
    {
        if (m_needsUpdate > 0)
        {
            m_needsUpdate = 0;
            glDeleteLists(m_displayList, 1);
            m_displayList = 0;
        }
    }

    PatchRenderObject& GetRenderObject(const Geometry::PatchRaw& patch)
    {
        PatchRenderObject* res = dynamic_cast<PatchRenderObject*>(patch->GetRenderObject());
        if (!res)
        {
            patch->SetRenderObject(std::make_unique<PatchRenderObject>());
            res = dynamic_cast<PatchRenderObject*>(patch->GetRenderObject());
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

} // Viewer
