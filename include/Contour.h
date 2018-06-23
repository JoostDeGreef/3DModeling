#pragma once

namespace Geometry
{
    /* contour: simple line shape in the x,y plane
    *
    * note:
    *   - the last and the first point are connected
    *   - selfintersections are not detected and/or corrected.
    *
    */
    class Contour 
    {
    public:
        Contour();
        Contour(const Contour& other);
        Contour(const std::vector<Vector2d>& points);

        void Add(const Vector2d& point);
        void Add(const std::vector<Vector2d>& points);

        const std::vector<Vector2d>& GetPoints() const { return m_points; }

        void ForceClockwise();
    private:
        std::vector<Vector2d> m_points;
    };
}


