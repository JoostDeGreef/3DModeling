#ifndef GEOMETRY_CONTOUR_H
#define GEOMETRY_CONTOUR_H 1

#include <functional>
#include <vector>

#include "Aliases.h"
#include "Vector.h"

namespace Geometry
{
    /* contour: line shape in the x,y plane
    *
    * note:
    *   - the last and the first point are connected
    *   - for now, selfintersections are not detected and/or corrected.
    *
    */
    class Contour : public SmallObjectAllocator<Contour>::Object
    {
    public:
        Contour();
        Contour(const Contour& other);
        Contour(const std::vector<Vector2d>& points);

        void Add(const Vector2d& point);
        void Add(const std::vector<Vector2d>& points);

        /* Turn the single line shape into several clockwise loops
        */
        std::vector<std::vector<Vector2d>> GetClockwiseLoops();
    private:
        std::vector<Vector2d> m_points;
    };
}

#endif // GEOMETRY_CONTOUR_H

