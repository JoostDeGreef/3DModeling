#pragma once

#include "Shape.h"

namespace Geometry
{
    class Dodecahedron : public Shape
    {
    public:
        Dodecahedron(const int initialFaceCount = 60);

    private:
        void InitialRefinement();
        void Refine(int initialFaceCount);
    };
};


