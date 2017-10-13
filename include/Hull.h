#ifndef GEOMETRY_HULL_H
#define GEOMETRY_HULL_H 1

namespace Geometry
{
    class Shape;

    class Hull 
    {
    public:
        typedef std::unordered_set<PatchPtr> container_type;
        typedef Hull this_type;
        typedef unsigned int size_type;

        enum class Orientation : unsigned char
        {
            Outward = 0,
            Inward = 1
        };
    protected:
        Shape& m_shape;
        Orientation m_orientation;
        container_type m_patches;
        BoundingShape3d m_boundingShape;

        Hull(Shape& shape)
            : m_shape(shape)
            , m_orientation(Orientation::Outward)
        {}

        // shallow copy only!
        Hull(const this_type &other) = default;
        Hull(this_type &&other) = default;
    public:
        Hull& operator = (const this_type &other) = delete;
        Hull& operator = (this_type &&other) = delete;

        ~Hull()
        {}

        // copies the hull into newShape
        HullPtr Hull::Copy(Shape& newShape) const;

        void AddPatch(PatchPtr& patch)
        {
            m_patches.emplace(patch);
        }
        void RemovePatch(PatchPtr& patch)
        {
            m_patches.erase(patch);
        }
        template<typename... Args>
        PatchPtr ConstructAndAddPatch(Args&&... args)
        {
            PatchPtr patch = Construct<Patch>(*this, std::forward<Args>(args)...);
            AddPatch(patch);
            return patch;
        }

        // Return the hull orientation
        Orientation GetOrientation() const { return m_orientation; }
        void SetOrientation(const Orientation& orientation) { m_orientation = orientation; }

        BoundingShape3d GetBoundingShape() const { return m_boundingShape; }
        void SetBoundingShape(const BoundingShape3d & boundingShape) { m_boundingShape = boundingShape; }

        void ForEachPatch(std::function<void(const PatchPtr& patch)> func) const;
        void ForEachFace(std::function<void(const FacePtr& facePtr)> func) const;
        void ForEachEdge(std::function<void(const EdgePtr& edgePtr)> func) const;

        // Get all patches in the hull
        const container_type& GetPatches() const { return m_patches; }
        container_type& GetPatches() { return m_patches; }

        // Split every edge in the hull; every triangle becomes 4 triangles
        void SplitTrianglesIn4();

        // Make sure the hull exist solely out of triangles
        void Triangulate();

        decltype(auto) GetShape() const { return m_shape; }
        //void SetShape(const ShapePtr& shape) { m_shape = shape; }
    };
}

#endif // GEOMETRY_HULL_H
