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

// algorithm:
// - create a sorted list of all vertices for both hulls
HullPtr Hull::Add(HullPtr & other)
{
    if (m_boundingShape.Touches(other->GetBoundingShape()))
    {

        // TODO

    }
    return HullPtr();
}

std::vector<HullPtr> Hull::Subtract(HullPtr & other)
{
    if (m_boundingShape.Touches(other->GetBoundingShape()))
    {

        // TODO

    }
    return std::vector<HullPtr>();
}

