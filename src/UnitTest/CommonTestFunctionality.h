#include "gtest/gtest.h"
using namespace testing;

#include "Aliases.h"
#include "Vector.h"
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

