#pragma once

namespace Geometry
{
    class Shape;

    class Hull : public std::enable_shared_from_this<Hull>
    {
    public:
        typedef std::unordered_set<FacePtr> container_type;
        typedef Hull this_type;
        typedef unsigned int size_type;

        enum class Orientation : unsigned char
        {
            Outward = 0,
            Inward = 1
        };
    private:
        ShapeRaw m_shape;
        Orientation m_orientation;
        container_type m_faces;
        BoundingShape3d m_boundingShape;

        ColorPtr m_color;
        RenderMode m_renderMode;
        std::shared_ptr<IRenderObject> m_renderObject;

        std::mutex m_mutex;

    protected:
        Hull(const ShapeRaw& shape)
            : Hull(shape, nullptr)
        {}
        Hull(const ShapeRaw& shape, const ColorPtr& color)
            : m_shape(shape)
            , m_orientation(Orientation::Outward)
            , m_boundingShape()
            , m_color(nullptr)
            , m_renderMode(RenderMode::Solid)
            , m_renderObject(std::make_unique<NOPRenderObject>())
        {}

        // shallow copy only!
        Hull(const this_type &other) = default;
        Hull(this_type &&other) = default;
    public:
        Hull& operator = (const this_type &other) = delete;
        Hull& operator = (this_type &&other) = delete;

        ~Hull()
        {
            ForcedInvalidate();
        }

        // copies the hull into newShape
        HullPtr Hull::Copy(Shape& newShape) const;

        const FacePtr& AddFace(const FacePtr& face) { return *m_faces.emplace(face).first; }
        const FacePtr& AddFace(const FaceRaw& face) { return AddFace(face.lock()); }
        void RemoveFace(const FacePtr& face) { m_faces.erase(face); }
        void RemoveFace(const FaceRaw& face) { RemoveFace(face.lock()); }
        template<typename... Args>
        const FacePtr& ConstructAndAddFace(Args&& ... args)
        {
            FacePtr face = Construct<Face>(this, std::forward<Args>(args)...);
            return AddFace(face);
        }

        // Return the hull orientation
        Orientation GetOrientation() const { return m_orientation; }
        void SetOrientation(const Orientation& orientation) { m_orientation = orientation; }

        // Invalidate makes cached render state in render object invalid.
        void Invalidate() { m_renderObject->Invalidate(); }
        void ForcedInvalidate() { m_renderObject->ForcedInvalidate(); }

        // RenderObject is a device dependent object state (for instance OpenGL displaylists).
        std::shared_ptr<IRenderObject> GetRenderObject() const { return m_renderObject; }
        void SetRenderObject(const std::shared_ptr<IRenderObject>& renderObject) { m_renderObject = renderObject; }

        // Color for the hull
        const ColorPtr& GetColor() const { return m_color; }
        void SetColor(const ColorPtr& color) { m_color = color; Invalidate(); }

        // Render mode
        const RenderMode GetRenderMode() const { return m_renderMode; }
        void SetRenderMode(const RenderMode renderMode) { m_renderMode = renderMode; Invalidate(); }

        // get/set/calculate/use the bounding shape
        const BoundingShape3d& GetBoundingShape() const { return m_boundingShape; }
        void SetBoundingShape(const BoundingShape3d& boundingShape) { m_boundingShape = boundingShape; /*Invalidate();*/ }
        void CalculateBoundingShape(const BoundingShape3d::Type type = BoundingShape3d::Type::Ball);
        bool BoundingShapesTouch(const this_type &other) const { return m_boundingShape.Touches(other.GetBoundingShape()); }

        // calculate the volume of the hull
        double CalculateVolume() const;

        // direct access to contained faces/edges
        const container_type& GetFaces() const { return m_faces; }
        std::unordered_set<VertexRaw> GetVertices() const;

        // access to the parent shape
        const ShapeRaw& GetShape() const { return m_shape; }

        // 'for each' type loops over all objects
        void ForEachFace(std::function<void(const FaceRaw& facePtr)> func) const;
        void ForEachEdge(std::function<void(const EdgeRaw& edgePtr)> func) const;
        void ForEachVertex(std::function<void(const VertexRaw& vertexPtr)> func) const;

        // scale every vertex
        void Scale(const double factor);

        // translate every vertex
        void Translate(const Vector3d& translation);

        // Split every edge in the hull; every triangle becomes 4 triangles
        void SplitTrianglesIn4();

        // Make sure the hull exist solely out of triangles
        void Triangulate();

        // geometry operations
        HullPtr Add(HullPtr& other);       // A joined with B, returns new hull or null if there is no overlap.
        std::vector<HullPtr> Subtract(HullPtr& other);  // A minus overlap with B, returns all resulting pieces (A and B if there is no overlap).

        // Make locking on this hull easy
        // examples:
        //    auto lock = hull.GetLock();
        // or
        //    auto lock = hull.TryGetLock();
        //    if( lock ) { ... }
        auto GetLock()
        { 
            return std::unique_lock<std::mutex>(m_mutex);
        }
        auto TryGetLock()
        {
            return std::unique_lock<std::mutex>(m_mutex,std::try_to_lock);
        }

    };
}

