#include <functional>
#include <unordered_map>

using namespace std;

#include "Geometry.h"
#include "GLWrappers.h"
#include "RenderObjects.h"

using namespace Geometry;

namespace Viewer
{

    HullRenderObject::~HullRenderObject()
    {
        SetDisplayList(0);
    }

    std::unordered_map<Geometry::HullRaw, std::shared_ptr<HullRenderObject>>& GetRenderObjects()
    {
        static std::unordered_map<Geometry::HullRaw, std::shared_ptr<HullRenderObject>> objects;
        return objects;
    }

    void HullRenderObject::SetDisplayList(unsigned int displayList)
    {
        glDeleteLists(m_displayList, 1);
        m_needsUpdate = 0;
        m_displayList = displayList;
    }

    HullRenderObject& GetRenderObject(const Geometry::HullRaw& hull)
    {
        auto& objects = GetRenderObjects();
        auto iter = objects.find(hull);
        if (iter == objects.end())
        {
            auto lock = hull->GetLock();
            auto p = Construct<HullRenderObject>();
            hull->SetRenderObject(p);
            iter = objects.emplace(hull, p).first;
        }
        return *iter->second;
    }

    void DisposeAllRenderObjects()
    {
        auto& objects = GetRenderObjects();
        objects.clear();
    }

    void HandleDisposedRenderObjects()
    {
        auto& objects = GetRenderObjects();
        for (size_t i=0;i<objects.size() && i<5;++i)
        {
            auto iter = std::next(std::begin(objects), Geometry::Numerics::NormalizedRandomNumber(objects.size()-1));
            if (iter->second.use_count() < 1)
            {
                objects.erase(iter);
            }
        }
    }
} // Viewer
