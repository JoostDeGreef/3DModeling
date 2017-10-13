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
    class Edge
    {
    private:
        VertexPtr m_startVertex;
        NormalPtr m_startNormal;
        FacePtr m_face;
        EdgePtr m_twin;
        EdgePtr m_next;
        EdgePtr m_prev;
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
        Edge(const FacePtr& face, const VertexPtr& vertex)
            : m_startVertex(std::move(vertex))
            , m_startNormal()
            , m_face(std::move(face))
            , m_twin()
            , m_next()
            , m_prev()
            , m_startColor()
            , m_startTextureCoord()
        {}
        Edge(const FacePtr& face, const VertexPtr& vertex, const NormalPtr& normal)
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

        decltype(auto) GetTwin() const { return m_twin; }
        void SetTwin(const decltype(m_twin)& twin) { m_twin = twin; }

        decltype(auto) GetStartVertex() const { return m_startVertex; }
        void SetStartVertex(const decltype(m_startVertex)& startVertex) { m_startVertex = startVertex; }
        decltype(auto) GetEndVertex() const { return GetTwin()->GetStartVertex(); }

        decltype(auto) GetStartNormal() const { return m_startNormal; }
        void SetStartNormal(const decltype(m_startNormal)& startNormal) { m_startNormal = startNormal; }
        decltype(auto) GetEndNormal() const { return GetTwin()->m_startNormal; }

        decltype(auto) GetFace() const { return m_face; }
        void SetFace(const decltype(m_face)& face) { m_face = face; }
        decltype(auto) GetTwinFace() const { return GetTwin()->m_face; }

        decltype(auto) GetNext() const { return m_next; }
        void SetNext(const decltype(m_next)& next) { m_next = next; }

        decltype(auto) GetPrev() const { return m_prev; }
        void SetPrev(const decltype(m_prev)& prev) { m_prev = prev; }

        decltype(auto) GetStartColor() const { return m_startColor; }
        void SetStartColor(const decltype(m_startColor)& startColor) { m_startColor = startColor; }
        decltype(auto) GetEndColor() const { return GetTwin()->GetStartColor(); }

        decltype(auto) GetStartTextureCoord() const { return m_startTextureCoord; }
        void SetStartTextureCoord(const decltype(m_startTextureCoord)& startTextureCoord) { m_startTextureCoord = startTextureCoord; }
        decltype(auto) GetEndTextureCoord() const { return GetTwin()->GetStartTextureCoord(); }

        void Split();

        void ForEachEdgeAtStartVertex(std::function<void(EdgePtr& edge)> func) const;

        double GetLength() const { return Geometry::Distance(*GetStartVertex(), *GetEndVertex()); }
    };

}

#endif // GEOMETRY_EDGE_H

