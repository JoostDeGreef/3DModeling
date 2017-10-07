#ifndef GEOMETRY_SHAPE_H
#define GEOMETRY_SHAPE_H 1

#include <functional>
#include <memory>
#include <unordered_set>

#include "Aliases.h"
#include "BoundingShape.h"
#include "Face.h"
#include "Hull.h"

namespace Geometry
{
    class Shape 
    {
    public:
        typedef std::unordered_set<HullPtr> container_type;
        typedef Shape this_type;
        typedef unsigned int size_type;
        typedef unsigned int index_type;
    protected:
        BoundingShape3d m_boundingShape;
        container_type m_hulls;

    public:

        Shape();
        Shape(const this_type &other);
        Shape(this_type &&other);

        template<typename... Args>
        static std::shared_ptr<this_type> Construct(Args&... args)
        {
            SmallObjectAllocator<this_type> allocator;
            return std::allocate_shared<this_type>(allocator, args...);
        }

        Shape& operator = (const this_type &other) = default;
        Shape& operator = (this_type &&other) = default;

        virtual ~Shape();

        void AddHull(HullPtr& hull)
        {
            m_hulls.emplace(hull);
        }
        void RemoveHull(HullPtr& hull)
        {
            m_hulls.erase(hull);
        }
        template<typename... Args>
        HullPtr ConstructAndAddHull(Args&... args)
        {
            HullPtr hull = Hull::Construct(*this,args...);
            AddHull(hull);
            return hull;
        }

        const container_type& GetHulls() const { return m_hulls; }

        void ForEachHull(std::function<void(const HullPtr& hull)> func) const
        {
            for (const HullPtr& hull : GetHulls()) { func(hull); }
        }
        void ForEachPatch(std::function<void(const PatchPtr& patch)> func) const
        {
            ForEachHull([func](const HullPtr& hull) { hull->ForEachPatch(func); });
        }
        void ForEachFace(std::function<void(const FacePtr& facePtr)> func) const
        {
            ForEachHull([func](const HullPtr& hull) { hull->ForEachFace(func); });
        }
        void ForEachEdge(std::function<void(const EdgePtr& edgePtr)> func) const
        {
            ForEachHull([func](const HullPtr& hull) { hull->ForEachEdge(func); });
        }
        void ForEachVertex(std::function<void(const VertexPtr& vertexPtr)> func) const
        {
            std::unordered_set<VertexPtr> vertices;
            ForEachFace([&vertices](const FacePtr& face) 
            {
                face->ForEachVertex([&vertices](const VertexPtr& vertex) 
                {
                    vertices.insert(vertex); 
                }); 
            });
            for (const VertexPtr& vertex : vertices)
            {
                func(vertex);
            }
        }

        // Split all edges in the shape and connect them; divide all triangles into 4 triangles.
        void SplitTrianglesIn4()
        {
            ForEachHull([](const HullPtr& hull) {hull->SplitTrianglesIn4(); });
        }

        // Make sure the shape exists solely out of triangles
        void Triangulate()
        {
            ForEachHull([](const HullPtr& hull) {hull->Triangulate(); });
        }

        const BoundingShape3d& GetBoundingShape() const { return m_boundingShape; }
        void SetBoundingShape(const BoundingShape3d& boundingShape) { m_boundingShape = boundingShape; }

        void Scale(const double factor);
        void Translate(const Vector3d& translation);
        //void Rotate(const Vector3d& origin, )

    protected:
        void Clear();
    };
}

#endif // GEOMETRY_SHAPE_H
