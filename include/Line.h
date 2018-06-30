#pragma once

#include <utility>

namespace Geometry
{
    // line segment
	template<typename POINT>
	class TLine 
	{
	public:
		typedef TLine<POINT> this_type;
		typedef POINT point_type;
        typedef typename point_type::value_type value_type;

		TLine(const point_type &p1,const point_type &p2)
			: m_p1(p1)
			, m_p2(p2)
		{}
		TLine(const this_type &other)
			: m_p1(other.m_p1)
			, m_p2(other.m_p2)
		{}

		const point_type &GetPoint1() const { return m_p1; }
		const point_type &GetPoint2() const { return m_p2; }

        class Intersection
        {
        public:
            enum Type
            {
                Unknown     = 0x00,
                
                Before      = 0x01,
                Start       = 0x02,
                On          = 0x04,
                End         = 0x08,
                After       = 0x10,

                Parallel    = 0x20,
                Colinear    = 0x40,
                Overlapping = 0x80,
            };

            Intersection(const Type type[2], const point_type& intersection, const double s[2])
                : m_type{ type[0], type[1] }
                , m_intersection(std::move(intersection))
                , m_s{ s[0], s[1] }
            {}

            const Type& GetType(const unsigned int index) const { return m_type[index]; }
            const point_type& GetIntersection() const { return m_intersection; }
// TODO: these are not correct
            bool IsParallel() const { return Parallel == m_type[0]; } // lines are parallel
            bool LinesIntersect() const { return Parallel != m_type[0]; } // lines are not parallel
            bool LinesSharePoint() const { return (m_type[0] & 0x0A) && (m_type[1] & 0x0A); } // end-end, start-start, end-start, start-end
            bool SegmentsIntersect() const { return (m_type[0] & 0x0E) && (m_type[1] & 0x0E); } // lines intersect within segments
            const double& s(const unsigned int index) const { return m_s[index]; }
        private:
            Type m_type[2];
            point_type m_intersection;
            double m_s[2];
        };

        Intersection CalculateIntersection(const TLine<POINT> &other, const value_type eps = 0.00001) const;
	private:
		point_type m_p1;
		point_type m_p2;
	};

    template<typename POINT>
    inline typename TLine<POINT>::Intersection TLine<POINT>::CalculateIntersection(const TLine<POINT>& other, const value_type eps) const
    {
        point_type slope0 = m_p2 - m_p1;
        point_type slope1 = other.m_p2 - other.m_p1;
        value_type den = slope0[0] * slope1[1] - slope0[1] * slope1[0];
        if (den > eps || -den > eps)
        {
            point_type pivot = m_p1 - other.m_p1;
            value_type s = (pivot[1] * slope0[0] - pivot[0] * slope0[1])/den;
            value_type t = (pivot[1] * slope1[0] - pivot[0] * slope1[1])/den;
            point_type intersection = ((m_p1 + slope0*t) + (other.m_p1 + slope1*s))*0.5;
            auto Interval = [](const value_type& s, const value_type& eps)
            {
                if (s + eps < 0) { return 1; }
                if (s <= eps) { return 2; }
                if (s + eps < 1) { return 4; }
                if (s <= 1 + eps) { return 8; }
                return 16;
            };
            const Intersection::Type types[2] = { static_cast<Intersection::Type>(Interval(t, eps*slope0.Length())), static_cast<Intersection::Type>(Interval(s, eps*slope1.Length())) };
            const value_type st[2] = { t,s };
            return Intersection(types, intersection, st);
        }
        else
        {
// TODO: handle colinear and overlapping
            const Intersection::Type types[2] = { Intersection::Parallel,Intersection::Parallel };
            const value_type st[2] = { -1,-1 };
            return Intersection(types, point_type(), st);
        }
    }

};


