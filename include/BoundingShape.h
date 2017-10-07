#ifndef BOUNDINGSHAPE_H
#define BOUNDINGSHAPE_H 1

#include <limits>

#include "MiniBall.h"
#include "Numerics.h"
#include "Operations.h"
#include "Vector.h"

namespace Geometry
{
    template<typename VECTOR_TYPE>
    class TBoundingShape 
    {
    public:
        typedef TBoundingShape<VECTOR_TYPE> this_type;
        typedef VECTOR_TYPE vector_type;
        typedef typename VECTOR_TYPE::value_type value_type;
        typedef unsigned int index_type;
        typedef unsigned int size_type;
        static const size_type dimension = vector_type::dimension;

        enum class Type : unsigned char
        {
            Unknown = 1,
            Ball    = 2,
            Box     = 3
        };

        TBoundingShape()
            : m_type(Type::Unknown)
            , m_optimal(false)
        {}
        TBoundingShape(const this_type &other)
        {
            Copy(other);
        }
        TBoundingShape(const vector_type &min_point, const vector_type &max_point)
        {
            Set(min_point, max_point);
        }
        TBoundingShape(const vector_type &center, const value_type &radius)
        {
            Set(center, radius);
        }
        template<typename... Args>
        static std::shared_ptr<this_type> Construct(Args&... args)
        {
            SmallObjectAllocator<this_type> allocator;
            return std::allocate_shared<this_type>(allocator, args...);
        }

        void Set(const vector_type &min_point, const vector_type &max_point, const bool optimal = true)
        {
            box.m_min = min_point;
            box.m_max = max_point;
            m_optimal = optimal;
            m_type = Type::Box;
        }

        void Set(const vector_type &center, const value_type &radius, const bool optimal = true)
        {
            ball.m_center = center;
            ball.m_radius = radius;
            m_optimal = optimal;
            m_type = Type::Ball;
        }

        Type GetType() const { return m_type; }

    private:
        template < typename Pit_, typename Cit_>
        struct MyCoordAccessor 
        {
            typedef Pit_ Pit;
            typedef Cit_ Cit;
            inline  Cit operator() (Pit it) const { return (Cit)(*(*it)).GetData(); }
        };
    public:
        template<typename ITERATOR>
        void Set(const Type type, ITERATOR itBegin, ITERATOR itEnd)
        {
            switch (type)
            {
            case Type::Unknown:
                m_type = type;
                break;
            case Type::Ball:
                {
                    Miniball::Miniball<MyCoordAccessor<ITERATOR, vector_type::value_type*> > mb(vector_type::dimension, itBegin, itEnd);
                    vector_type v(mb.center());
                    Set(v, sqrt(mb.squared_radius()));
                }
                break;
            case Type::Box:
                {
                    vector_type vmin, vmax;
                    vmin.Fill(Numerics::Limits<vector_type::value_type>::MaxValue);
                    vmax.Fill(Numerics::Limits<vector_type::value_type>::MinValue);
                    for (ITERATOR it = itBegin; it != itEnd; ++it)
                    {
                        const Vertex & v = **it;
                        for (Vertex::index_type i = 0; i < vector_type::dimension; ++i)
                        {
                            if (v[i] < vmin[i]) vmin[i] = v[i];
                            if (v[i] > vmax[i]) vmax[i] = v[i];
                        }
                    }
                    Set(vmin, vmax);
                }
                break;
            }
        }

        this_type &operator = (const this_type &other)
        {
            Copy(other);
            return *this;
        }

        void Convert(const Type type)
        {
            if (m_type == type)
            {
                return;
            }
            if (m_type != Type::Unknown && type != Type::Unknown)
            {
                switch (m_type)
                {
                case Type::Ball:
                    switch (type)
                    {
                    case Type::Box:
                        {
                            Box newBox;
                            vector_type offset;
                            offset.Fill(ball.m_radius);
                            newBox.m_min = ball.m_center - offset;
                            newBox.m_max = ball.m_center + offset;
                            box = newBox;
                            m_optimal = false;
                            m_type = type;
                        }
                        return;
                    }
                    break;
                case Type::Box:
                    switch (type)
                    {
                    case Type::Ball:
                        {
                            Ball newBall;
                            newBall.m_center = Middle(box.m_min, box.m_max);
                            newBall.m_radius = Distance(box.m_min, box.m_max) / 2;
                            ball = newBall;
                            m_optimal = false;
                            m_type = type;
                        }
                        return;
                    }
                    break;
                }
                assert(false); // m_type or type is garbage
            }
            m_type = Type::Unknown;
            m_optimal = false;
        }

        vector_type GetCenter() const
        {
            switch (m_type)
            {
            default:
            case Type::Unknown:
            case Type::Box:
                assert(false);
            case Type::Ball:
                break;
            }
            return ball.m_center;
        }
        double GetRadius() const
        {
            switch (m_type)
            {
            default:
            case Type::Unknown:
            case Type::Box:
                assert(false);
            case Type::Ball:
                break;
            }
            return ball.m_radius;
        }
        vector_type GetMin() const
        {
            switch (m_type)
            {
            default:
            case Type::Unknown:
            case Type::Ball:
                assert(false);
            case Type::Box:
                break;
            }
            return box.m_min;
        }
        vector_type GetMax() const
        {
            switch (m_type)
            {
            default:
            case Type::Unknown:
            case Type::Ball:
                assert(false);
            case Type::Box:
                break;
            }
            return box.m_max;
        }

        void Copy(const this_type &other)
        {
            m_type = other.m_type;
            m_optimal = other.m_optimal;
            switch (m_type)
            {
            default:
                box.m_min = other.box.m_min;
                box.m_max = other.box.m_max;
                break;
            case Type::Ball:
                ball.m_center = other.ball.m_center;
                ball.m_radius = other.ball.m_radius;
                break;
            }
        }

        bool Touches(const this_type &other) const
        {
            if (m_type == other.m_type)
            {
                switch (m_type)
                {
                default:
                    assert(false);
                case Type::Box:
                    return Touches(box, other.box);
                case Type::Ball:
                    return Touches(ball, other.ball);
                }
            }
            else
            {
                switch (m_type)
                {
                default:
                    assert(false);
                case Type::Box:
                    return Touches(box, other.ball);
                case Type::Ball:
                    return Touches(other.box, ball);
                }
            }
        }

        bool Encapsulates(const vector_type& value) const
        {
            switch (m_type)
            {
            default:
                //assert(false);
            case Type::Box:
                return Encapsulates(box, value);
            case Type::Ball:
                return Encapsulates(ball, value);
            }
        }

        bool IsInitialized() const
        {
            return m_type != Type::Unknown;
        }

        bool IsOptimal() const
        {
            return m_optimal;
        }

    private:
        struct Box
        {
            vector_type m_min;
            vector_type m_max;
        };
        struct Ball
        {
            vector_type m_center;
            value_type m_radius;
        };

        Type m_type;
        bool m_optimal;
        union
        {
            Box box;
            Ball ball;
        };

        static bool Encapsulates(const Box& box, const vector_type& value)
        {
            bool res = true;
            for (index_type i = 0; i < dimension; ++i)
            {
                res = res && value[i] >= box.m_min[i] && value[i] <= box.m_max[i];
            }
            return res;
        }

        static bool Encapsulates(const Ball& ball, const vector_type& value)
        {
            return DistanceSquared(ball.m_center, value) <= ball.m_radius*ball.m_radius;
        }

        static bool Touches(const Box &b0, const Box &b1)
        {
            bool res = true;
            for (index_type i = 0; i < dimension; ++i)
            {
                res = res && b0.m_min[i] <= b1.m_max[i] && b0.m_max[i] >= b1.m_min[i];
            }
            return res;
        }

        static bool Touches(const Ball &b0, const Ball &b1)
        {
            return DistanceSquared(b0.m_center, b1.m_center) <= pow(b0.m_radius + b1.m_radius, 2);
        }

        template<unsigned int coord>
        static bool TouchesCoord(const Box &box, const Ball &ball, vector_type& corner, bool contained)
        {
            const vector_type& center = ball.m_center;
            const value_type& radius = ball.m_radius;
            const vector_type& min = box.m_min;
            const vector_type& max = box.m_max;
            if (center[coord] + radius < min[coord])
            {
                return false;
            }
            else if (center[coord] < min[coord])
            {
                corner[coord] = min[coord];
                return TouchesCoord<coord + 1>(box,ball,corner,false);
            }
            else if (center[coord] <= max[coord])
            {
                corner[coord] = center[coord];
                return TouchesCoord<coord + 1>(box, ball, corner, contained);
            }
            else if (center[coord] - radius <= max[coord])
            {
                corner[coord] = max[coord];
                return TouchesCoord<coord + 1>(box, ball, corner, false);
            }
            else
            {
                return false;
            }
        };
        template<>
        static bool TouchesCoord<dimension>(const Box &box, const Ball &ball, vector_type& corner, bool contained)
        {
            return contained || Encapsulates(ball, corner);
        }
        static bool Touches(const Box &box, const Ball &ball)
        {
            vector_type corner;
            bool contained = true;
            return TouchesCoord<0>(box,ball,corner,contained);
        }

    };

};

#endif // BOUNDINGSHAPE_H
