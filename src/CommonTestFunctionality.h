#include "gtest/gtest.h"
using namespace testing;

#include "Aliases.h"
#include "Contour.h"
#include "Numerics.h"
#include "Vector.h"
#include "Quaternion.h"
#include "RGBAColor.h"
#include "BoundingShape.h"
using namespace Geometry;
using namespace Geometry::Numerics;

inline std::ostream& operator<<(std::ostream& stream, Vector2d const& v)
{
    return stream << "Vector2d(" << v[0] << "," << v[1] << ")";
}

inline std::ostream& operator<<(std::ostream& stream, Vector3d const& v)
{
    return stream << "Vector3d(" << v[0] << "," << v[1] << "," << v[2] << ")";
}

