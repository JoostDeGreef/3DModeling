#include <functional>
#include <unordered_map>

using namespace std;

#include "Geometry.h"
#include "GLWrappers.h"
#include "RenderInfo.h"

using namespace Geometry;

namespace Viewer
{
    RenderInfo::RenderInfo()
    {
        static Color color(1, 0, 0, 1);
        Update(&color);
        Update(RenderMode::Solid);
    }

    void RenderInfo::Push(const Geometry::ShapeRaw & shape)
    {
        m_stack.emplace(m_state);
    }

    void RenderInfo::Push(const Geometry::HullRaw & hull)
    {
        m_stack.emplace(m_state);
        Update(hull->GetColor().get());
        Update(hull->GetRenderMode());
    }

    void RenderInfo::Push(const Geometry::FaceRaw & face)
    {
        m_stack.emplace(m_state);
        Update(face->GetColor().get());
    }

    void RenderInfo::Push(const Geometry::EdgeRaw & edge)
    {
        m_stack.emplace(m_state);
        Update(edge->GetStartColor().get());
    }

    void RenderInfo::Pop()
    {
        const State& state = m_stack.top();
        Update(state.m_color);
        Update(state.m_renderMode);
        m_stack.pop();
    }

    void RenderInfo::Update(const Color* color)
    {
        if (color && color != m_state.m_color)
        {
            glColor(*color);
            m_state.m_color = color;
        }
    }

    void RenderInfo::Update(const Geometry::RenderMode renderMode)
    {
        if (renderMode != m_state.m_renderMode)
        {
            switch (renderMode)
            {
            case RenderMode::Transparent:
                //assert(false); // Transparancy not implemented yet.
                break;
            case RenderMode::Solid:
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glEnable(GL_CULL_FACE);
                break;
            case RenderMode::WireFrame:
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glDisable(GL_CULL_FACE);
                break;
            }
            m_state.m_renderMode = renderMode;
        }
    }

} // Viewer
