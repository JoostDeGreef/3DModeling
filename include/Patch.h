#ifndef GEOMETRY_PATCH_H
#define GEOMETRY_PATCH_H 1

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

    protected:
        Patch(Hull& hull)
            : Patch(hull, Construct<Color>(Color::White()))
        {}
        Patch(Hull& hull, const ColorPtr& color)
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

        void AddFace(FacePtr& face)
        {
            m_faces.emplace(face);
        }
        void RemoveFace(FacePtr& face)
        {
            m_faces.erase(face);
        }
        template<typename... Args>
        FacePtr ConstructAndAddFace(Args&& ... args)
        {
            FacePtr face = Construct<Face>(*this, std::forward<Args>(args)...);
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
        std::unordered_set<VertexPtr> GetVertices() const;

        void ForEachFace(std::function<void(const FacePtr& facePtr)> func) const;
        void ForEachEdge(std::function<void(const EdgePtr& edgePtr)> func) const;
        void ForEachVertex(std::function<void(const VertexPtr& vertexPtr)> func) const;

        const Hull& GetHull() const { return m_hull; }
        const Shape& GetShape() const;
        //void SetHull(const HullPtr& hull) { m_hull = hull; }
    };
}

#endif // GEOMETRY_PATCH_H
