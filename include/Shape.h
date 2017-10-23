#ifndef GEOMETRY_SHAPE_H
#define GEOMETRY_SHAPE_H 1

namespace Geometry
{
    class Shape : std::enable_shared_from_this<Shape>
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

        Shape& operator = (const this_type &other) = default;
        Shape& operator = (this_type &&other) = default;

        virtual ~Shape();

        const HullPtr& AddHull(const HullPtr& hull)
        {
            return *m_hulls.emplace(hull).first;
        }
        const HullPtr& AddHull(const HullRaw& hull)
        {
            return AddHull(hull.lock());
        }
        void RemoveHull(const HullPtr& hull)
        {
            m_hulls.erase(hull);
        }
        void RemoveHull(const HullRaw& hull)
        {
            RemoveHull(hull.lock());
        }
        template<typename... Args>
        const HullPtr& ConstructAndAddHull(Args&&... args)
        {
            HullPtr hull = Construct<Hull>(this,std::forward<Args>(args)...);
            return AddHull(hull);
        }

        const container_type& GetHulls() const { return m_hulls; }

        void ForEachHull(std::function<void(const HullRaw& hull)> func) const;
        void ForEachPatch(std::function<void(const PatchRaw& patch)> func) const;
        void ForEachFace(std::function<void(const FaceRaw& facePtr)> func) const;
        void ForEachEdge(std::function<void(const EdgeRaw& edgePtr)> func) const;
        void ForEachVertex(std::function<void(const VertexRaw& vertexPtr)> func) const;

        // Split all edges in the shape and connect them; divide all triangles into 4 triangles.
        void SplitTrianglesIn4();

        // Make sure the shape exists solely out of triangles
        void Triangulate();

        const BoundingShape3d& GetBoundingShape() const { return m_boundingShape; }
        void SetBoundingShape(const BoundingShape3d& boundingShape) { m_boundingShape = boundingShape; }

        void Scale(const double factor);
        void Translate(const Vector3d& translation);
        //void Rotate(const Vector3d& origin, )

        // Store/Retrieve shape to db.
        void Store(SQLite::DB& db) const;
        void Retrieve(SQLite::DB& db);
    protected:
        void Clear();
    };
}

#endif // GEOMETRY_SHAPE_H
