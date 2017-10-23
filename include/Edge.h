#ifndef GEOMETRY_EDGE_H
#define GEOMETRY_EDGE_H 1

namespace Geometry
{
    /* Edge : full edge, linked using face+index
     *
     *      end
     *       |
     *  left | right
     *       |
     *     start
     * 
     * The constructors of this class are protected to force usage of 'Construct<Edge>(...)'
     *
     */
    class Edge : public std::enable_shared_from_this<Edge>
    {
    private:
        VertexPtr m_startVertex;
        NormalPtr m_startNormal;
        FaceRaw m_face;
        EdgeRaw m_twin;
        EdgeRaw m_next;
        EdgeRaw m_prev;
        ColorPtr m_startColor;
        TextureCoordPtr m_startTextureCoord;
    public:
        typedef Edge this_type;

    protected:
        Edge()
            : m_startVertex()
            , m_startNormal()
            , m_face()
            , m_twin()
            , m_next()
            , m_prev()
            , m_startColor()
            , m_startTextureCoord()
        {}
        Edge(const FaceRaw& face, const VertexPtr& vertex)
            : m_startVertex(std::move(vertex))
            , m_startNormal()
            , m_face(std::move(face))
            , m_twin()
            , m_next()
            , m_prev()
            , m_startColor()
            , m_startTextureCoord()
        {}
        Edge(const FaceRaw& face, const VertexPtr& vertex, const NormalPtr& normal)
            : m_startVertex(std::move(vertex))
            , m_startNormal(std::move(normal))
            , m_face(std::move(face))
            , m_twin()
            , m_next()
            , m_prev()
            , m_startColor()
            , m_startTextureCoord()
        {}
        Edge(const this_type &other) = default;
        Edge(this_type &&other) = default;

    public:
        ~Edge()
        {}

        Edge& operator = (const this_type &other) = delete;
        Edge& operator = (this_type &&other) = delete;

        EdgeRaw GetTwin() const { return m_twin; }
        void SetTwin(const EdgeRaw& twin) { m_twin = twin; }

        const VertexPtr& GetStartVertex() const { return m_startVertex; }
        void SetStartVertex(const VertexPtr& startVertex) { m_startVertex = startVertex; }
        const VertexPtr& GetEndVertex() const { return GetTwin()->GetStartVertex(); }

        const NormalPtr& GetStartNormal() const { return m_startNormal; }
        void SetStartNormal(const NormalPtr& startNormal) { m_startNormal = startNormal; }
        const NormalPtr& GetEndNormal() const { return GetTwin()->m_startNormal; }

        const FaceRaw& GetFace() const { return m_face; }
        void SetFace(const FaceRaw& face) { m_face = face; }
        const FaceRaw& GetTwinFace() const { return GetTwin()->m_face; }

        EdgeRaw GetNext() const { return m_next; }
        void SetNext(const EdgeRaw& next) { m_next = next; }

        EdgeRaw GetPrev() const { return m_prev; }
        void SetPrev(const EdgeRaw& prev) { m_prev = prev; }

        const ColorPtr& GetStartColor() const { return m_startColor; }
        void SetStartColor(const ColorPtr& startColor) { m_startColor = startColor; }
        const ColorPtr& GetEndColor() const { return GetTwin()->GetStartColor(); }

        const TextureCoordPtr& GetStartTextureCoord() const { return m_startTextureCoord; }
        void SetStartTextureCoord(const TextureCoordPtr& startTextureCoord) { m_startTextureCoord = startTextureCoord; }
        const TextureCoordPtr& GetEndTextureCoord() const { return GetTwin()->GetStartTextureCoord(); }

        void Split();

        void ForEachEdgeAtStartVertex(std::function<void(const EdgeRaw& edge)> func) const;

        double GetLength() const { return Geometry::Distance(*GetStartVertex(), *GetEndVertex()); }
    };

}

#endif // GEOMETRY_EDGE_H

