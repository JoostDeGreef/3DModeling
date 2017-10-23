#ifndef GEOMETRY_PATCH_H
#define GEOMETRY_PATCH_H 1

namespace Geometry
{
    class Patch : public std::enable_shared_from_this<Patch>
    {
    public:
        typedef std::unordered_set<FacePtr> container_type;
        typedef Patch this_type;
        typedef unsigned int size_type;
    private:
        HullRaw m_hull;
        container_type m_faces;
        ColorPtr m_color;
        unsigned int m_displayList;
        unsigned int m_textureId;
        BoundingShape3d m_boundingShape;

    protected:
        Patch(const HullRaw& hull)
            : Patch(hull, Construct<Color>(Color::White()))
        {}
        Patch(const HullRaw& hull, const ColorPtr& color)
            : m_hull(hull)
            , m_color(color)
            , m_displayList(0)
            , m_textureId(0)
            , m_boundingShape()
        {}

        Patch(const this_type &other) = default;
        Patch(this_type &&other) = default;
    public:
        Patch& operator = (const this_type &other) = delete;
        Patch& operator = (this_type &&other) = delete;

        const FacePtr& AddFace(const FacePtr& face)
        {
            return *m_faces.emplace(face).first;
        }
        const FacePtr& AddFace(const FaceRaw& face)
        {
            return AddFace(face.lock());
        }
        void RemoveFace(const FacePtr& face)
        {
            m_faces.erase(face);
        }
        void RemoveFace(const FaceRaw& face)
        {
            RemoveFace(face.lock());
        }
        template<typename... Args>
        const FacePtr& ConstructAndAddFace(Args&& ... args)
        {
            FacePtr face = Construct<Face>(this, std::forward<Args>(args)...);
            return AddFace(face);
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
        std::unordered_set<VertexRaw> GetVertices() const;

        void ForEachFace(std::function<void(const FaceRaw& facePtr)> func) const;
        void ForEachEdge(std::function<void(const EdgeRaw& edgePtr)> func) const;
        void ForEachVertex(std::function<void(const VertexRaw& vertexPtr)> func) const;

        const HullRaw& GetHull() const { return m_hull; }
        const ShapeRaw& GetShape() const;
    };
}

#endif // GEOMETRY_PATCH_H
