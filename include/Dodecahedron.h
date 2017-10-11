#ifndef GEOMETRY_DODECAHEDRON_H
#define GEOMETRY_DODECAHEDRON_H 1

#include "Shape.h"

namespace Geometry
{
    class Dodecahedron : public Shape
    {
    public:
        Dodecahedron(const int initialFaceCount = 60);

        template<typename... Args>
        static std::shared_ptr<Shape> Construct(const Args&... args)
        {
            SmallObjectAllocator<Dodecahedron> allocator;
            return std::allocate_shared<Dodecahedron>(allocator, args...);
        }

    private:
        void InitialRefinement();
        void Refine(int initialFaceCount);
    };
};

#endif // GEOMETRY_DODECAHEDRON_H

