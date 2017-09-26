#include <cassert>
#include <vector>
using namespace std;

#include "Aliases.h"
#include "RGBAColor.h"
#include "Patch.h"
#include "Hull.h"
#include "Shape.h"
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

