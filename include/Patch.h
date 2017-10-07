#ifndef GEOMETRY_PATCH_H
#define GEOMETRY_PATCH_H 1

#include <functional>
#include <memory>
#include <unordered_set>

#include "Aliases.h"
#include "BoundingShape.h"
#include "RGBAColor.h"
#include "Face.h"


namespace Geometry
{
    class Patch
    {
    public:
        typedef std::unordered_set<FacePtr> container_type;
        typedef Patch this_type;
        typedef unsigned int size_type;
    private:
        Hull& m_hull;
        container_type m_faces;
        ColorPtr m_color;
        unsigned int m_displayList;
        unsigned int m_textureId;
        BoundingShape3d m_boundingShape;
    public:

        Patch(Hull& hull)
            : Patch(hull, Color::Construct(Color::White()))
        {}
        Patch(Hull& hull, const ColorPtr& color)
            : m_hull(hull)
            , m_color(color)
            , m_displayList(0)
            , m_textureId(0)
            , m_boundingShape()
        {}
        template<typename... Args>
        static std::shared_ptr<this_type> Construct(Args&... args)
        {
            SmallObjectAllocator<this_type> allocator;
            return std::allocate_shared<this_type>(allocator, args...);
        }

        Patch(const this_type &other) = default;
        Patch(this_type &&other) = default;
        Patch& operator = (const this_type &other) = default;
        Patch& operator = (this_type &&other) = default;

        void AddFace(FacePtr& face)
        {
            m_faces.emplace(face);
        }
        void RemoveFace(FacePtr& face)
        {
            m_faces.erase(face);
        }
        template<typename... Args>
        FacePtr ConstructAndAddFace(Args& ... args)
        {
            FacePtr face = Face::Construct(*this, args...);
            AddFace(face);
            return face;
        }

        const ColorPtr& GetColor() const { return m_color; }
        void SetColor(const ColorPtr& color) { m_color = color; }

        const BoundingShape3d& GetBoundingShape() const { return m_boundingShape; }
        void SetBoundingShape(const BoundingShape3d& boundingShape) { m_boundingShape = boundingShape; }
        void CalculateBoundingShape();

        unsigned int GetTextureId() const { return m_textureId; }
        void SetTextureId(const unsigned int textureId) { m_textureId = textureId; }

        unsigned int GetDisplayList() const { return m_displayList; }
        void SetDisplayList(const unsigned int displayList) { m_displayList = displayList; }

        const container_type& GetFaces() const { return m_faces; }

        void ForEachFace(std::function<void(const FacePtr& facePtr)> func) const
        {
            for (const FacePtr& face:m_faces)
            {
                func(face);
            }
        }

        void ForEachEdge(std::function<void(const EdgePtr& edgePtr)> func) const
        {
            ForEachFace([func](const FacePtr & facePtr)
            {
                facePtr->ForEachEdge(func);
            });
        }

        std::unordered_set<VertexPtr> GetVertices() const
        {
            std::unordered_set<VertexPtr> vertices;
            ForEachFace([&vertices](const FacePtr& face) 
            {
                face->ForEachVertex([&vertices](const VertexPtr& vertex) 
                {
                    vertices.insert(vertex); 
                }); 
            });
            return vertices;
        }
        void ForEachVertex(std::function<void(const VertexPtr& vertexPtr)> func) const
        {
            for (const VertexPtr& vertex : GetVertices())
            {
                func(vertex);
            }
        }

        const Hull& GetHull() const { return m_hull; }
        const Shape& GetShape() const;
        //void SetHull(const HullPtr& hull) { m_hull = hull; }
    };
}

#endif // GEOMETRY_PATCH_H
