#ifndef LINE_H
#define LINE_H 1

#include <utility>

namespace Geometry
{
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
        template<typename... Args>
        static std::shared_ptr<this_type> Construct(Args&... args)
        {
            SmallObjectAllocator<this_type> allocator;
            return std::allocate_shared<this_type>(allocator, args...);
        }

		const point_type &GetPoint1() const { return m_p1; }
		const point_type &GetPoint2() const { return m_p2; }

        class Intersection
        {
        public:
            enum Type
            {
                Parallel     = 0x0000, // 0
                
                BeforeBefore = 0x0101, // 257
                BeforeStart  = 0x0102, // 258
                BeforeOn     = 0x0104, // 260
                BeforeEnd    = 0x0108, // 264
                BeforeAfter  = 0x0110, // 272

                StartBefore  = 0x0201, // 513
                StartStart   = 0x0202, // 514
                StartOn      = 0x0204, // 516
                StartEnd     = 0x0208, // 520
                StartAfter   = 0x0210, // 528

                OnBefore     = 0x0401, // 1025
                OnStart      = 0x0402, // 1026
                OnOn         = 0x0404, // 1028
                OnEnd        = 0x0408, // 1032
                OnAfter      = 0x0410, // 1040

                EndBefore    = 0x0801, // 2049
                EndStart     = 0x0802, // 2050
                EndOn        = 0x0804, // 2052
                EndEnd       = 0x0808, // 2056
                EndAfter     = 0x0810, // 2064

                AfterBefore  = 0x1001, // 4097
                AfterStart   = 0x1002, // 4098
                AfterOn      = 0x1004, // 4100
                AfterEnd     = 0x1008, // 4104
                AfterAfter   = 0x1010  // 4112
            };

            Intersection(const Type& type, const point_type& intersection)
                : m_type(std::move(type))
                , m_intersection(std::move(intersection))
            {}

            const Type& GetType() const { return m_type; }
            const point_type& GetIntersection() const { return m_intersection; }
            bool IsParallel() { return Parallel == m_type; } // lins are parallel
            bool LinesIntersect() { return Parallel != m_type; } // lines are not parallel
            bool LinesSharePoint() { return (m_type & 0x0A00) && (m_type & 0x000A); } // end-end, start-start, end-start, start-end
            bool SegmentsIntersect() { return (m_type & 0x0E00) && (m_type & 0x000E); } // lines intersect within segments
        private:
            Type m_type;
            point_type m_intersection;
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
            int type = Interval(t, eps*slope0.Length()) * 256 + Interval(s, eps*slope1.Length());
            return Intersection(static_cast<Intersection::Type>(type), intersection);
        }
        else
        {
            return Intersection(Intersection::Parallel,point_type());
        }
    }
};


#endif // LINE_H
