#include "Geometry.h"
using namespace std;
using namespace Geometry;

void Patch::CalculateBoundingShape()
{
    const auto& vertices = GetVertices();
    m_boundingShape.Set(BoundingShape3d::Type::Ball, vertices.begin(), vertices.end());
}

const Shape& Patch::GetShape() const
{
    return GetHull().GetShape();
}

std::unordered_set<VertexPtr> Patch::GetVertices() const
{
    std::unordered_set<VertexPtr> vertices;
    ForEachFace([&vertices](const FacePtr& face)
    {
        face->ForEachVertex([&vertices](const VertexPtr& vertex)
        {
            vertices.insert(vertex);
        });
    });
    return vertices;
}

void Patch::ForEachFace(std::function<void(const FacePtr& facePtr)> func) const
{
    for (const FacePtr& face : m_faces)
    {
        func(face);
    }
}

void Patch::ForEachEdge(std::function<void(const EdgePtr& edgePtr)> func) const
{
    ForEachFace([func](const FacePtr & facePtr)
    {
        facePtr->ForEachEdge(func);
    });
}

void Patch::ForEachVertex(std::function<void(const VertexPtr& vertexPtr)> func) const
{
    for (const VertexPtr& vertex : GetVertices())
    {
        func(vertex);
    }
}
