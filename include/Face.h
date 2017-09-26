#ifndef GEOMETRY_FACE_H
#define GEOMETRY_FACE_H 1

#include <functional>
#include <memory>
#include <unordered_set>

#include "Aliases.h"
#include "Edge.h"

namespace Geometry
{
    /* Face : full edge indexed winged face
     */
    class Face 
    {
    public:
        typedef Face this_type;
        typedef std::unordered_set<EdgePtr> container_type;        
    private:
        Patch& m_patch;
        container_type m_edges;
        NormalPtr m_normal;
        ColorPtr m_color;
    public:

        Face(Patch& patch)
            : m_edges()
            , m_normal()
            , m_patch(patch)
        {}

        Face(const this_type &other) = default;
        Face(this_type &&other) = default;
        Face& operator = (const this_type &other) = default;
        Face& operator = (this_type &&other) = default;

        ~Face();

        void AddEdge(EdgePtr& edge)
        {
            m_edges.emplace(edge);
        }
        void RemoveEdge(EdgePtr& edge)
        {
            m_edges.erase(edge);
        }
        template<typename... Args>
        static EdgePtr ConstructAndAddEdge(FacePtr& newFace, const Args& ... args)
        {
            EdgePtr edge = std::make_shared<Edge>(newFace, args...);
            newFace->AddEdge(edge);
            return edge;
        }

        decltype(auto) GetNormal() const { return m_normal; }
        void SetNormal(const decltype(m_normal)& normal) { m_normal = normal; }
        void CalcNormal();

        const EdgePtr& GetStartEdge() const { return *m_edges.begin(); }
        const container_type& GetEdgesUnordered() const { return m_edges; }
        const std::vector<EdgePtr> GetEdgesOrdered() const;

        const ColorPtr& GetColor() const { return m_color; }
        void SetColor(const ColorPtr& color) { m_color = color; }

        const Patch& GetPatch() const { return m_patch; }
        Patch& GetPatch() { return m_patch; }
        //void SetPatch(const PatchPtr& patch) { m_patch = patch; }

        size_t GetEdgeCount() const { return m_edges.size(); }

        void ForEachEdge(std::function<void(const EdgePtr& edge)> func) const
        {
            const EdgePtr startEdge = GetStartEdge();
            EdgePtr edge = startEdge;
            do
            {
                func(edge);
                edge = edge->GetNext();
            } while (edge != startEdge);
        }
        void ForEachVertex(std::function<void(const VertexPtr& vertex)> func) const
        {
            ForEachEdge([func](const EdgePtr& edge)
            {
                func(edge->GetStartVertex());
            });
        }

        // Split the face in 2; may add vertices
        std::pair<FacePtr, FacePtr> Split();

        // Make sure the face only consist out of triangles; only uses existing vertices
        void Triangulate();

#ifdef _DEBUG
        void CheckPointering() const;
#else  // _DEBUG
        void CheckPointering() const {}
#endif // _DEBUG

    };
};

#endif // GEOMETRY_FACE_H

