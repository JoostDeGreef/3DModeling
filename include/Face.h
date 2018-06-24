#pragma once

namespace Geometry
{
    /* Face : full edge indexed winged face
     */
    class Face : public std::enable_shared_from_this<Face>
    {
    public:
        typedef Face this_type;
        typedef std::unordered_set<EdgePtr> container_type;        
    private:
        HullRaw m_hull;
        container_type m_edges;
        NormalPtr m_normal;
        ColorPtr m_color;

    protected:
        Face(const HullRaw& hull)
            : m_edges()
            , m_normal()
            , m_hull(hull)
        {}
        Face(const this_type &other) = default;
        Face(this_type &&other) = default;

    public:
        Face& operator = (const this_type &other) = delete;
        Face& operator = (this_type &&other) = delete;

        ~Face();

        const EdgePtr& AddEdge(EdgePtr& edge) { return *m_edges.emplace(edge).first; }
        const EdgePtr& AddEdge(EdgeRaw& edge) { return AddEdge(edge.lock()); }
        void RemoveEdge(EdgePtr& edge) { m_edges.erase(edge); }
        void RemoveEdge(EdgeRaw& edge) { RemoveEdge(edge.lock()); }
        template<typename... Args>
        const EdgePtr& ConstructAndAddEdge(Args&&... args)
        {
            EdgePtr edge = Construct<Edge>(this, std::forward<Args>(args)...);
            return AddEdge(edge);
        }

        const NormalPtr& GetNormal() const { return m_normal; }
        void SetNormal(const NormalPtr& normal) { m_normal = normal; }
        void CalcNormal();

        const EdgePtr& GetStartEdge() const { return *m_edges.begin(); }
        const container_type& GetEdgesUnordered() const { return m_edges; }
        const std::vector<EdgeRaw> GetEdgesOrdered() const;

        const ColorPtr& GetColor() const { return m_color; }
        void SetColor(const ColorPtr& color) { m_color = color; }

        const HullRaw& GetHull() const { return m_hull; }

        size_t GetEdgeCount() const { return m_edges.size(); }

        void ForEachEdge(std::function<void(const EdgeRaw& edge)> func) const;
        void ForEachVertex(std::function<void(const VertexRaw& vertex)> func) const;

        // Split the face in 2; may add vertices
        std::pair<FacePtr, FacePtr> Split();

        // Make sure the face only consist out of triangles; only uses existing vertices
        void Triangulate();

        // Obtain the intersetions pointf for a line in the face plane with the contour of the face.
        class ContourLineIntersection
        {
        public:

        };
        std::vector<ContourLineIntersection> GetContourLineIntersections(const Vertex& dir, const Vertex & point) const;

        // Find intersection between faces
        class FaceIntersection
        {
            friend Face;
        public:
            struct IntersectionPoint
            {
                EdgePtr m_edgeA;
                EdgePtr m_edgeB;
                VertexPtr m_vertex;
            };

            FaceIntersection(FacePtr& A, FacePtr& B)
                : m_A(A)
                , m_B(B)
            {}

            operator bool() const { return !m_points.empty(); }

            void Calculate();
        protected:
            FacePtr m_A;
            FacePtr m_B;

            std::vector<IntersectionPoint> m_points;
        };
        FaceIntersection FindIntersection(FacePtr& other);

#ifdef _DEBUG
        void CheckPointering() const;
#else  // _DEBUG
        void CheckPointering() const {}
#endif // _DEBUG

    };
};


