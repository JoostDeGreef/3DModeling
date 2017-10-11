#ifndef GEOMETRY_ROTATIONMATRIX_H
#define GEOMETRY_ROTATIONMATRIX_H 1

namespace Geometry
{
    template<typename VALUE_TYPE>
    class TRotationMatrix3 : public SmallObjectAllocator<TRotationMatrix3<VALUE_TYPE>>::Object
    {
    public:
        typedef VALUE_TYPE value_type;
        typedef TRotationMatrix3<VALUE_TYPE> this_type;
        typedef TQuaternion<VALUE_TYPE> quad_type;
        typedef TVector<VALUE_TYPE, 3> vector_type;
        typedef unsigned int index_type;
        typedef unsigned int size_type;

    private:
        vector_type m_data[3];

    public:
        TRotationMatrix3()
        {
            Set(1, 0, 0, 
                0, 1, 0,
                0, 0, 1);
        }
        TRotationMatrix3(const this_type &other)
        {
            Copy(other);
        }
        TRotationMatrix3(const quad_type &quad)
        {
            quad.GetRotationMatrix3rows(m_data[0].GetData(),m_data[1].GetData(),m_data[2].GetData());
        }

        this_type &operator = (const this_type &other)
        {
            Copy(other);
            return *this;
        }

        void Copy(const this_type &other)
        {
            m_data[0] = other.m_data[0];
            m_data[1] = other.m_data[1];
            m_data[2] = other.m_data[2];
        }

        void Set(const value_type &a0, const value_type &a1, const value_type &a2, 
                 const value_type &b0, const value_type &b1, const value_type &b2, 
                 const value_type &c0, const value_type &c1, const value_type &c2)
        {
            m_data[0].set(a0, a1, a2);
            m_data[1].set(b0, b1, b2);
            m_data[2].set(c0, c1, c2);
        }

        bool operator == (const this_type &other) const
        {
            return 
                m_data[0] == other.m_data[0] &&
                m_data[1] == other.m_data[1] &&
                m_data[2] == other.m_data[2];
        }

        const value_type &operator [] (const index_type &index) const
        {
            return m_data[index];
        }
        value_type &operator [] (const index_type &index)
        {
            return m_data[index];
        }

        vector_type Transform(const vector_type& v) const
        {
            return vector_type(m_data[0].InnerProduct(v), m_data[1].InnerProduct(v), m_data[2].InnerProduct(v));
        }
    };
};

#endif // GEOMETRY_ROTATIONMATRIX_H
