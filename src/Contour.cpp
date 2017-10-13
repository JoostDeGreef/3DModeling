
using namespace std;

#include "Geometry.h"

using namespace Geometry;

namespace
{
    // this should probably be replaced by a sweepline algorithm, 
    // but for contours with small linecount, it's Okay...
    std::vector<std::vector<Vector2d>> SplitAtSelfintersections(const std::vector<Vector2d>& points)
    {
        //assert(points.size() >= 3);
        std::vector<std::vector<Vector2d>> res;
        //std::vector<Line2d> lines;
        //auto cur = points.cbegin();
        //lines.emplace_back(points.back(), *cur);
        //auto prev = cur++;
        //for (; cur != points.cend(); ++cur, ++prev)
        //{
        //    lines.emplace_back(*prev, *cur);
        //    for (size_t li = 0; li + 2 < lines.size(); ++li)
        //    {

        //    }
        //}

        if (!points.empty())
        {
            res.push_back(points);
        }
        return res;
    }

    void TurnClockwise(std::vector<Vector2d>& points)
    {
        double dir = 0;
        Vector2d p0 = points.back();
        for(const Vector2d& p1:points)
        {
            dir += p0[0] * p1[1] - p0[1] * p1[0];
            p0 = p1;
        }
        if (dir > 0)
        {
            reverse(points.begin(), points.end());
        }
    }
}

Contour::Contour()
    : m_points()
{}

Contour::Contour(const Contour & other)
    : m_points(other.m_points)
{}

Contour::Contour(const std::vector<Vector2d>& points)
    : m_points(points)
{}

void Contour::Add(const Vector2d & point)
{
    m_points.emplace_back(point);
}

void Contour::Add(const std::vector<Vector2d>& points)
{
    m_points.insert(m_points.end(),points.begin(),points.end());
}

std::vector<std::vector<Vector2d>> Contour::GetClockwiseLoops()
{
    std::vector<std::vector<Vector2d>> res = SplitAtSelfintersections(m_points);
    for (auto& loop : res)
    {
        TurnClockwise(loop);
    }
    return res;
}
