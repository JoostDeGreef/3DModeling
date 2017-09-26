#ifndef GEOMETRY_VECTOR_H
#define GEOMETRY_VECTOR_H 1

#include <array>
#include <memory>

namespace Geometry
{
	template<typename VALUE_TYPE, unsigned int DIMENSION>
    class TVector
    {
    public:
        typedef TVector<VALUE_TYPE, DIMENSION> this_type;
        typedef VALUE_TYPE value_type;
        typedef unsigned int index_type;
        typedef unsigned int size_type;
        static const size_type dimension = DIMENSION;

    private:
        std::array<value_type, dimension> m_data;

    public:
        TVector()
        {}
        TVector(const this_type &other)
        {
            Copy(other);
        }
        template<typename ...Args>
        TVector(const value_type &value,Args ...values)
        {
            Set(value, values...);
        }
        explicit TVector(const value_type *data)
        {
            Set(data);
        }

        this_type &operator = (const this_type &other)
        {
            Copy(other);
            return *this;
        }

        void Copy(const this_type &other)
        {
            m_data = other.m_data;
        }

    private:
        template<index_type index>
        void SetIndex(const value_type &value)
        {
            m_data[index] = value;
        }
        template<index_type index, typename ...Args>
        void SetIndex(const value_type &value, Args ...values)
        {
            m_data[index] = value;
            SetIndex<index + 1>(values...);
        }
    public:
        template<typename ...Args>
        void Set(const value_type &value, Args ...values)
        {
            static_assert(sizeof...(values)+1 == dimension, "Incorrect number of values");
            SetIndex<0>(value, values...);
        }
        void Set(const value_type *data)
        {
            for (index_type i = 0; i < dimension; ++i)
            {
                m_data[i] = data[i];
            }
        }

        void Fill(const value_type &value)
        {
            for (index_type i = 0; i < dimension; ++i)
            {
                m_data[i] = value;
            }
        }

        const value_type* GetData() const
        {
            return m_data.data();
        }
        value_type* GetData() 
        {
            return m_data.data();
        }

    private:
        template<index_type index>
        bool IsEqualIndex(const this_type &other) const
        {
            return Numerics::Equal(m_data[index], other.m_data[index]) && IsEqualIndex<index + 1>(other);
        }
        template<>
        bool IsEqualIndex<dimension>(const this_type &other) const
        {
            return true;
        }
    public:
        bool operator == (const this_type &other) const
        {
            return IsEqualIndex<0>(other);
        }

        const value_type &operator [] (const index_type &index) const
        {
            return m_data[index];
        }
        value_type &operator [] (const index_type &index)
        {
            return m_data[index];
        }

        value_type InnerProduct(const this_type &other) const
        {
            value_type res = m_data[0] * other[0];
            for (index_type i = 1; i < dimension; ++i)
            {
                res += m_data[i] * other[i];
            }
            return res;
        }

        this_type OuterProduct(const this_type &other) const
        {
            this_type res;
            for (index_type i = 0; i < dimension; ++i)
            {
                res[i] = m_data[i] * other[i];
            }
            return res;
        }

        value_type Length() const
        {
            return sqrt(LengthSquared());
        }
        value_type LengthSquared() const
        {
            value_type res = pow(m_data[0],2);
            for (index_type i = 1; i < dimension; ++i)
            {
                res += pow(m_data[i],2);
            }
            return res;
        }

        this_type &Normalize() 
        {
            return *this /= Length();
        }
        this_type Normalized() const
        {
            value_type length = Length();
            assert(length > 0.000001);
            return *this / length;
        }

        this_type operator + (const this_type &other) const
        {
            this_type res;
            for (index_type i = 0; i < dimension; ++i)
            {
                res[i] = m_data[i] + other[i];
            }
            return res;
        }
        this_type &operator += (const this_type &other) 
        {
            for (index_type i = 0; i < dimension; ++i)
            {
                m_data[i] += other[i];
            }
            return *this;
        }

        this_type operator - (const this_type &other) const
        {
            this_type res;
            for (index_type i = 0; i < dimension; ++i)
            {
                res[i] = m_data[i] - other[i];
            }
            return res;
        }
        this_type &operator -= (const this_type &other) 
        {
            for (index_type i = 0; i < dimension; ++i)
            {
                m_data[i] -= other[i];
            }
            return *this;
        }

        this_type operator * (const value_type &value) const
        {
            this_type res;
            for (index_type i = 0; i < dimension; ++i)
            {
                res[i] = m_data[i] * value;
            }
            return res;
        }
        this_type &operator *= (const value_type &value) 
        {
            for (index_type i = 0; i < dimension; ++i)
            {
                m_data[i] *= value;
            }
            return *this;
        }

        this_type operator / (const value_type &value) const
        {
            this_type res;
            for (index_type i = 0; i < dimension; ++i)
            {
                res[i] = m_data[i] / value;
            }
            return res;
        }
        this_type &operator /= (const value_type &value) 
        {
            for (index_type i = 0; i < dimension; ++i)
            {
                m_data[i] /= value;
            }
            return *this;
        }
    };

    template<typename VALUE_TYPE>
    inline TVector3<VALUE_TYPE> CrossProduct(const TVector3<VALUE_TYPE> &a, const TVector3<VALUE_TYPE> &b) 
    {
        return  TVector3<VALUE_TYPE>(
            a[1] * b[2] - a[2] * b[1],
            a[2] * b[0] - a[0] * b[2],
            a[0] * b[1] - a[1] * b[0]);
    }

    template<typename VALUE_TYPE>
    inline VALUE_TYPE ScalarTripleProduct(const TVector3<VALUE_TYPE> &a,const TVector3<VALUE_TYPE> &b,const TVector3<VALUE_TYPE> &c)
    {
        return a.InnerProduct(CrossProduct(b,c));
    }

    template<typename VALUE_TYPE>
    inline TVector3<VALUE_TYPE> VectorTripleProduct(const TVector3<VALUE_TYPE> &a,const TVector3<VALUE_TYPE> &b,const TVector3<VALUE_TYPE> &c)
    {
        return CrossProduct(a,CrossProduct(b,c));
    }
};

#endif // GEOMETRY_VECTOR_H
