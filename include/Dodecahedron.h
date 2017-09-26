#ifndef GEOMETRY_DODECAHEDRON_H
#define GEOMETRY_DODECAHEDRON_H 1

#include "Shape.h"

namespace Geometry
{
    class Dodecahedron : public Shape
    {
    public:
        Dodecahedron(int initialFaceCount = 60);

    private:
        void InitialRefinement();
        void Refine(int initialFaceCount);
    };
};

#endif // GEOMETRY_DODECAHEDRON_H

