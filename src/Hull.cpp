#include "Geometry.h"
using namespace std;
using namespace Geometry;

HullPtr Hull::Copy(Shape& newShape) const
{
    ForEachFace([](const FaceRaw& face) { face->CheckPointering(); });

    HullPtr newHull = newShape.ConstructAndAddHull();
    newHull->SetOrientation(GetOrientation());
    newHull->SetBoundingShape(GetBoundingShape());
    newHull->SetColor(GetColor());
    newHull->SetRenderMode(GetRenderMode());

    std::unordered_map<FaceRaw, FacePtr> faces;
    std::unordered_map<EdgeRaw, EdgePtr> edges;
    std::unordered_map<VertexRaw, VertexPtr> vertices;
    std::unordered_map<NormalRaw, NormalPtr> normals;
    std::unordered_map<ColorRaw, ColorPtr> colors;
    std::unordered_map<TextureCoordRaw, TextureCoordPtr> textureCoordinates;

    // create a new instance for each existing patch, face, edge, vertex, normal, color and texturecoordinate
    colors.emplace(GetColor(), nullptr);
    ForEachFace([&](const FaceRaw& face)
    {
        normals.emplace(face->GetNormal(), nullptr);
        colors.emplace(face->GetColor(), nullptr);
        faces.emplace(face, nullptr);
        face->ForEachEdge([&](const EdgeRaw& edge)
        {
            edges.emplace(edge, nullptr);
            vertices.emplace(edge->GetStartVertex(), nullptr);
            normals.emplace(edge->GetStartNormal(), nullptr);
            colors.emplace(edge->GetStartColor(), nullptr);
            textureCoordinates.emplace(edge->GetStartTextureCoord(), nullptr);
        });
    });
    // duplicate shallow structures
    for (auto& vertexMap : vertices)
    {
        vertexMap.second = vertexMap.first == nullptr ? nullptr : Construct<Vertex>(*vertexMap.first);
    }
    for (auto& normalMap : normals)
    {
        normalMap.second = normalMap.first == nullptr ? nullptr : Construct<Normal>(*normalMap.first);
    }
    for (auto& colorMap : colors)
    {
        colorMap.second = colorMap.first == nullptr ? nullptr : Construct<Color>(*colorMap.first);
    }
    for (auto& textureCoordinateMap : textureCoordinates)
    {
        textureCoordinateMap.second = textureCoordinateMap.first == nullptr ? nullptr : Construct<TextureCoord>(*textureCoordinateMap.first);
    }
    // recreate complex structures
    for (auto& faceMap : faces)
    {
        faceMap.second = newHull->ConstructAndAddFace();
    }
    for (auto& edgeMap : edges)
    {
        VertexPtr& v = vertices[edgeMap.first->GetStartVertex()];
        assert(v != nullptr);
        FacePtr& f = faces[edgeMap.first->GetFace()];
        assert(f != nullptr);
        edgeMap.second = Construct<Edge>(f,v);
        f->AddEdge(edgeMap.second);
    }
    for (auto& edgeMap : edges)
    {
        edgeMap.second->SetStartNormal(normals[edgeMap.first->GetStartNormal()]);
        edgeMap.second->SetTwin(edges[edgeMap.first->GetTwin()]);
        edgeMap.second->SetNext(edges[edgeMap.first->GetNext()]);
        edgeMap.second->SetPrev(edges[edgeMap.first->GetPrev()]);
        edgeMap.second->SetStartColor(colors[edgeMap.first->GetStartColor()]);
        edgeMap.second->SetStartTextureCoord(textureCoordinates[edgeMap.first->GetStartTextureCoord()]);
    }
    for (auto& faceMap : faces)
    {
        assert(faceMap.first->GetEdgeCount() == faceMap.second->GetEdgeCount());
        assert(faceMap.second->GetStartEdge() != nullptr);
        faceMap.second->SetNormal(normals[faceMap.first->GetNormal()]);
        faceMap.second->SetColor(colors[faceMap.first->GetColor()]);
    }
    newHull->ForEachFace([](const FaceRaw& face) { face->CheckPointering(); });
    return newHull;
}

void Hull::SplitTrianglesIn4()
{
    size_t count = 0;
    std::vector<FaceRaw> faces;
    std::unordered_set<EdgeRaw> edges;
    ForEachFace([&count](const FaceRaw& face) 
    {
        ++count;
        face->CheckPointering(); 
    });
    faces.reserve(count);
    ForEachFace([&edges,&faces](const FaceRaw& face)
    {
        assert(3==face->GetEdgeCount());
        faces.emplace_back(face);
        face->ForEachEdge([&](const EdgeRaw& edge)
        {
            if (edge < edge->GetTwin())
            {
                edges.insert(edge);
            }
        });
    });
    for (const EdgeRaw& edge : edges)
    {
        edge->Split();
    }
    for (FaceRaw& face : faces)
    {
        // new faces
        FacePtr f0 = ConstructAndAddFace();
        FacePtr f1 = ConstructAndAddFace();
        FacePtr f2 = ConstructAndAddFace();
        FacePtr f3 = ConstructAndAddFace();
        // edges, with e01 pre-existing
        EdgeRaw e01 = face->GetStartEdge();
        if (edges.find(e01) == edges.end())
        {
            e01 = e01->GetNext();
        }
        EdgeRaw e12 = e01->GetNext();
        EdgeRaw e23 = e12->GetNext();
        EdgeRaw e34 = e23->GetNext();
        EdgeRaw e45 = e34->GetNext();
        EdgeRaw e50 = e45->GetNext();
        // newly created vertices
        VertexPtr v1 = e12->GetStartVertex();
        VertexPtr v3 = e34->GetStartVertex();
        VertexPtr v5 = e50->GetStartVertex();
        // normals at new vertices
        NormalPtr n1 = e12->GetStartNormal();
        NormalPtr n3 = e34->GetStartNormal();
        NormalPtr n5 = e50->GetStartNormal();
        // create new edges
        EdgePtr e13 = f3->ConstructAndAddEdge(v1, n1);
        EdgePtr e31 = f1->ConstructAndAddEdge(v3, n3);
        EdgePtr e35 = f3->ConstructAndAddEdge(v3, n3);
        EdgePtr e53 = f2->ConstructAndAddEdge(v5, n5);
        EdgePtr e15 = f0->ConstructAndAddEdge(v1, n1);
        EdgePtr e51 = f3->ConstructAndAddEdge(v5, n5);
        // set new edge twins
        e13->SetTwin(e31); e31->SetTwin(e13);
        e35->SetTwin(e53); e53->SetTwin(e35);
        e15->SetTwin(e51); e51->SetTwin(e15);
        // set next/prev
        auto Link = [](EdgeRaw e0, EdgeRaw e1) {e0->SetNext(e1); e1->SetPrev(e0); };
        Link(e01, e15); Link(e15, e50);
        Link(e23, e31); Link(e31, e12);
        Link(e45, e53); Link(e53, e34);
        Link(e13, e35); Link(e35, e51); Link(e51, e13);
        // connect existing edges to new face
        e01->SetFace(f0); e50->SetFace(f0);
        e12->SetFace(f1); e23->SetFace(f1);
        e34->SetFace(f2); e45->SetFace(f2);
        // add existing edges to faces
        f0->AddEdge(e01);
        f0->AddEdge(e50);
        f1->AddEdge(e12);
        f1->AddEdge(e23);
        f2->AddEdge(e34);
        f2->AddEdge(e45);
        // calculate face normals
        f0->CalcNormal();
        f1->CalcNormal();
        f2->CalcNormal();
        f3->CalcNormal();
        // check all faces
        f0->CheckPointering();
        f1->CheckPointering();
        f2->CheckPointering();
        f3->CheckPointering();
        // remove old face
        RemoveFace(face);
    }
}

void Hull::Triangulate()
{
    std::vector<FaceRaw> faces;
    ForEachFace([&faces](const FaceRaw& face) 
    {
        faces.emplace_back(face);
        face->CheckPointering();
    });
    for (FaceRaw& face : faces)
    {
        face->Triangulate();
    }
}

void Hull::CalculateBoundingShape(const BoundingShape3d::Type type)
{
    const auto& vertices = GetVertices();
    m_boundingShape.Set(type, vertices.begin(), vertices.end());
}

std::unordered_set<VertexRaw> Hull::GetVertices() const
{
    std::unordered_set<VertexRaw> vertices;
    ForEachFace([&vertices](const FaceRaw& face)
    {
        face->ForEachVertex([&vertices](const VertexRaw& vertex)
        {
            vertices.emplace(vertex);
        });
    });
    return vertices;
}

void Hull::ForEachFace(std::function<void(const FaceRaw& facePtr)> func) const
{
    for (const FaceRaw& face : m_faces)
    {
        func(face);
    }
}

void Hull::ForEachEdge(std::function<void(const EdgeRaw& edgePtr)> func) const
{
    ForEachFace([func](const FaceRaw & facePtr)
    {
        facePtr->ForEachEdge(func);
    });
}

void Hull::ForEachVertex(std::function<void(const VertexRaw& vertexPtr)> func) const
{
    for (const VertexRaw& vertex : GetVertices())
    {
        func(vertex);
    }
}

void Hull::Scale(const double factor)
{
    ForEachVertex([factor](const VertexRaw& vertex)
    {
        (*vertex) *= factor;
    });
}

void Hull::Translate(const Vector3d& translation)
{
    ForEachVertex([translation](const VertexRaw& vertex)
    {
        (*vertex) += translation;
    });
}

double Hull::CalculateVolume() const
{
    auto SignedVolumeOfTriangle = [](const Vertex& v1, const Vertex& v2, const Vertex& v3) 
    {
        auto v321 = v3[0]*v2[1]*v1[2];
        auto v231 = v2[0]*v3[1]*v1[2];
        auto v312 = v3[0]*v1[1]*v2[2];
        auto v132 = v1[0]*v3[1]*v2[2];
        auto v213 = v2[0]*v1[1]*v3[2];
        auto v123 = v1[0]*v2[1]*v3[2];
        return (1.0f / 6.0f)*(-v321 + v231 + v312 - v132 - v213 + v123);
    };
    auto SignedVolumeOfFace = [&SignedVolumeOfTriangle](const Face& face)
    {
        double volume = 0.0;
        assert(face.GetEdgeCount() > 2);
        EdgeRaw edgePtr = face.GetStartEdge();
        const auto& v1 = edgePtr->GetStartVertex();
        edgePtr = edgePtr->GetNext();
        auto v2 = edgePtr->GetStartVertex();
        edgePtr = edgePtr->GetNext();
        auto v3 = edgePtr->GetStartVertex();
        while (v3 != v1)
        {
            volume += SignedVolumeOfTriangle(*v1, *v2, *v3);
            v2 = v3;
            edgePtr = edgePtr->GetNext();
            v3 = edgePtr->GetStartVertex();
        }
        return volume;
    };
    double volume = 0.0;
    ForEachFace([&volume,&SignedVolumeOfFace](const FaceRaw& facePtr)
    {
        volume += SignedVolumeOfFace(*facePtr);
    });
    return fabs(volume);
}

// algorithm:
// - create a Z-sorted list of all vertices for both hulls
class HullConnector
{
    struct FaceInfo
    {
        FaceInfo(const FacePtr& faceptr, unsigned int id)
            : face(faceptr)
            , id(id)
        {}
        FacePtr face;
        unsigned int id; // 1,2
    };
    typedef std::vector<FaceInfo> Faces;
    typedef std::unordered_set<FaceInfo*> FacePtrs;
    struct VertexInfo
    {
        VertexInfo(const VertexRaw& vertex, const FacePtrs& faces, unsigned int id)
            : vertex(vertex)
            , faces(faces)
            , id(id)
        {}
        VertexInfo(const VertexRaw& vertex, const FacePtrs& faces)
            : VertexInfo(vertex, faces, (*(faces.begin()))->id)
        {}
        VertexInfo(const VertexRaw& vertex, unsigned int id = 3)
            : VertexInfo(vertex, {}, id)
        {}
        VertexRaw vertex;
        FacePtrs faces;
        unsigned int id; // 1,2  3=new
    };
    typedef std::vector<VertexInfo> Vertices;

public:
    HullConnector(Hull& a, Hull& b)
        : m_A(a)
        , m_B(b)
        , m_faces()
        , m_vertices()
        , m_res()
    {}

    // if any face in A is intersected by a face in B,
    // add the faces from B to A and create intersection edges.
    bool Connect()
    {
        bool intersection = false;
        if (m_A.BoundingShapesTouch(m_B))
        {
            // create sorted list of vertices
            m_faces.reserve(m_A.GetFaces().size() + m_B.GetFaces().size());
            GetFaceInfo(m_faces, m_A, 1);
            GetFaceInfo(m_faces, m_B, 2);
            m_vertices = GetVerticesFromFaces(m_faces);
            SortVerticesZXYId(m_vertices);
            // iterate over the vertices
            FacePtrs faces;
            while (!m_vertices.empty())
            {
                VertexInfo vertexinfo = m_vertices.back();
                m_vertices.pop_back();
                for (auto& face : vertexinfo.faces)
                {
                    if (faces.find(face) == faces.end())
                    {
                        intersection = SplitByFaceIfNeeded(faces,face) || intersection;
                    }
                }
            }
        }
        return intersection;
    }

protected:
    bool SplitByFaceIfNeeded(FacePtrs& faces, FaceInfo* face)
    {
        bool res = false;
        for (auto& f : faces)
        {
            auto intersection = f->face->FindIntersection(face->face);


            const auto& n0 = f->face->GetNormal();
            const auto& n1 = face->face->GetNormal();
            assert(n0 && n1);

            // todo

        }
        return res;
    }

    static void GetFaceInfo(Faces& faces, Hull& hull, const unsigned int id)
    {
        auto& hull_faces = hull.GetFaces();
        for (auto& hf : hull_faces)
        {
            faces.emplace_back(hf,id);
        }
    }

    // returns all the vertices, unsorted
    static Vertices GetVerticesFromFaces(Faces& faces)
    {
        std::unordered_multimap<VertexRaw, FaceInfo*> vertexFaceMap;
        for (auto& face : faces)
        {
            face.face->ForEachVertex([&vertexFaceMap,&face](const VertexRaw& vertex)
            {
                vertexFaceMap.emplace(vertex,&face);
            });
        }
        Vertices vertices;
        vertices.reserve(vertexFaceMap.bucket_count());
        FacePtrs faceInfoPtrs;
        VertexRaw vertex = nullptr;
        auto StoreInfo = [&vertices,&faceInfoPtrs](VertexRaw& vertex)
        {
            if (vertex)
            {
                vertices.emplace_back(vertex, faceInfoPtrs);
            }
            faceInfoPtrs.clear();
        };
        for (auto& vfm : vertexFaceMap)
        {
            if (vfm.first != vertex)
            {
                StoreInfo(vertex);
                vertex = vfm.first;
            }
            faceInfoPtrs.emplace(vfm.second);
        }
        StoreInfo(vertex);
        return vertices;
    }

    // sort the vertices by Z,X,Y,Id ascending
    static void SortVerticesZXYId(Vertices & vertices)
    {
        std::sort(vertices.begin(), vertices.end(), [](const VertexInfo & a, const VertexInfo & b) 
        {
            auto& va = *a.vertex;
            auto& vb = *b.vertex;
            if (va[2] < vb[2]) return true;
            if (va[2] > vb[2]) return false;
            if (va[0] < vb[0]) return true;
            if (va[0] > vb[0]) return false;
            if (va[1] < vb[1]) return true;
            if (va[1] > vb[1]) return false;
            return a.id < b.id;
        });
    }
private:
    Hull & m_A;
    Hull & m_B;
    Faces m_faces;
    Vertices m_vertices;
    std::vector<HullPtr> m_res;
};

HullPtr Hull::Add(HullPtr & other)
{
    HullConnector hc(*this, *other);
    if (hc.Connect())
    {

    }
    return HullPtr();
}

std::vector<HullPtr> Hull::Subtract(HullPtr & other)
{
    HullConnector hc(*this, *other);
    if (hc.Connect())
    {

    }
    return std::vector<HullPtr>();
}

