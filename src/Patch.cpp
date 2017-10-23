#include "Geometry.h"
using namespace std;
using namespace Geometry;

void Patch::CalculateBoundingShape()
{
    const auto& vertices = GetVertices();
    m_boundingShape.Set(BoundingShape3d::Type::Ball, vertices.begin(), vertices.end());
}

const ShapeRaw& Patch::GetShape() const
{
    return GetHull()->GetShape();
}

std::unordered_set<VertexRaw> Patch::GetVertices() const
{
    std::unordered_set<VertexRaw> vertices;
    ForEachFace([&vertices](const FaceRaw& face)
    {
        face->ForEachVertex([&vertices](const VertexRaw& vertex)
        {
            vertices.insert(vertex);
        });
    });
    return vertices;
}

void Patch::ForEachFace(std::function<void(const FaceRaw& facePtr)> func) const
{
    for (const FaceRaw& face : m_faces)
    {
        func(face);
    }
}

void Patch::ForEachEdge(std::function<void(const EdgeRaw& edgePtr)> func) const
{
    ForEachFace([func](const FaceRaw & facePtr)
    {
        facePtr->ForEachEdge(func);
    });
}

void Patch::ForEachVertex(std::function<void(const VertexRaw& vertexPtr)> func) const
{
    for (const VertexRaw& vertex : GetVertices())
    {
        func(vertex);
    }
}
