#ifndef GEOMETRY_CUBE_H
#define GEOMETRY_CUBE_H 1

#include "Shape.h"

namespace Geometry
{
    class Cube : public Shape
    {
    public:
        Cube();

        template<typename... Args>
        static std::shared_ptr<Shape> Construct(Args&... args)
        {
            SmallObjectAllocator<Cube> allocator;
            return std::allocate_shared<Cube>(allocator, args...);
        }

    };
};

#endif // GEOMETRY_CUBE_H

