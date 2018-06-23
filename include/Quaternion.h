#pragma once

namespace Geometry
{
    template<typename VALUE_TYPE>
    class TQuaternion 
    {
    public:
        typedef TQuaternion<VALUE_TYPE> this_type;
        typedef VALUE_TYPE value_type;
        typedef TVector3<VALUE_TYPE> vector_type;
        typedef unsigned int index_type;
        typedef unsigned int size_type;

    private:
        value_type m_data[4];

    public:
        TQuaternion()
        {
            Set(1, 0, 0, 0);
        }
        TQuaternion(const this_type &other)
        {
            Copy(other);
        }
        TQuaternion(const value_type &w, const value_type &x, const value_type &y, const value_type &z)
        {
            Set(w, x, y, z);
        }
        TQuaternion(const vector_type &axis, const value_type &angle)
        {
            Set(axis, angle);
        }
        TQuaternion(const vector_type &axis0, const vector_type &axis1)
        {
            vector_type rotAxis = CrossProduct(axis0, axis1);
            value_type angle = rotAxis.Length();
            rotAxis.Normalize();
            Set(rotAxis,angle);
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
            m_data[3] = other.m_data[3];
        }

        void Set(const vector_type &axis, const value_type &angle)
        {
            value_type sha = sin(angle*0.5);
            Set(cos(angle*0.5), axis[0] * sha, axis[1] * sha, axis[2] * sha);
        }
        void Set(const value_type &w, const value_type &x, const value_type &y, const value_type &z)
        {
            m_data[0] = w;
            m_data[1] = x;
            m_data[2] = y;
            m_data[3] = z;
        }

        const value_type &GetW() const
        {
            return m_data[0];
        }
        const value_type &GetX() const
        {
            return m_data[1];
        }
        const value_type &GetY() const
        {
            return m_data[2];
        }
        const value_type &GetZ() const
        {
            return m_data[3];
        }
        const value_type* GetWXYZ() const
        {
            return m_data;
        }

        bool operator == (const this_type &other) const
        {
            return 
                Numerics::Equal(m_data[0], other.m_data[0]) &&
                Numerics::Equal(m_data[1], other.m_data[1]) &&
                Numerics::Equal(m_data[2], other.m_data[2]) &&
                Numerics::Equal(m_data[3], other.m_data[3]);
        }

        const value_type &operator [] (const index_type &index) const
        {
            return m_data[index];
        }
        value_type &operator [] (const index_type &index)
        {
            return m_data[index];
        }

        value_type MagnitudeSqr() const
        {
            return Numerics::Sqr(m_data[0]) + Numerics::Sqr(m_data[1]) + Numerics::Sqr(m_data[2]) + Numerics::Sqr(m_data[3]);
        }

        value_type Magnitude() const
        {
            return sqrt(MagnitudeSqr());
        }

        value_type InnerProduct(const this_type& other) const
        {
            return 
                m_data[0] * other.m_data[0] + 
                m_data[1] * other.m_data[1] + 
                m_data[2] * other.m_data[2] + 
                m_data[3] * other.m_data[3];
        }
        value_type Dot(const this_type& other) const
        {
            return InnerProduct(other);
        }

        void Invert()
        {
            value_type scale = 1.0 / MagnitudeSqr();
            m_data[0] *=  scale;
            m_data[1] *= -scale;
            m_data[2] *= -scale;
            m_data[3] *= -scale;
        }
        this_type Inverted() const
        {
            value_type scale = 1.0 / MagnitudeSqr();
            return this_type(m_data[0] * scale, -m_data[1] * scale, -m_data[2] * scale, -m_data[3] * scale);
        }

        void Normalize()
        {
            *this /= Magnitude();
        }
        this_type Normalized() const
        {
            return *this / Magnitude();
        }

        this_type operator * (const value_type& value) const
        {
            return this_type(value*m_data[0], value*m_data[1], value*m_data[2], value*m_data[3]);
        }
        this_type& operator *= (const value_type& value) 
        {
            m_data[0] *= value;
            m_data[1] *= value;
            m_data[2] *= value;
            m_data[3] *= value;
            return *this;
        }

        this_type operator / (const value_type& value) const
        {
            return *this * (1 / value);
        }
        this_type& operator /= (const value_type& value) 
        {
            return *this *= (1 / value);
        }

        this_type operator + (const this_type& other) const
        {
            return this_type(m_data[0] + other.m_data[0], m_data[1] + other.m_data[1], m_data[2] + other.m_data[2], m_data[3] + other.m_data[3]);
        }
        this_type& operator += (const this_type& other)
        {
            m_data[0] += other.m_data[0];
            m_data[1] += other.m_data[1];
            m_data[2] += other.m_data[2];
            m_data[3] += other.m_data[3];
            return *this;
        }

        this_type operator - (const this_type& other) const
        {
            return this_type(m_data[0] - other.m_data[0], m_data[1] - other.m_data[1], m_data[2] - other.m_data[2], m_data[3] - other.m_data[3]);
        }
        this_type& operator -= (const this_type& other)
        {
            m_data[0] -= other.m_data[0];
            m_data[1] -= other.m_data[1];
            m_data[2] -= other.m_data[2];
            m_data[3] -= other.m_data[3];
            return *this;
        }

        this_type operator * (const this_type& other) const
        {
            return this_type(
                m_data[0] * other.m_data[0] - m_data[1] * other.m_data[1] - m_data[2] * other.m_data[2] - m_data[3] * other.m_data[3],
                m_data[0] * other.m_data[1] + m_data[1] * other.m_data[0] + m_data[2] * other.m_data[3] - m_data[3] * other.m_data[2],
                m_data[0] * other.m_data[2] - m_data[1] * other.m_data[3] + m_data[2] * other.m_data[0] + m_data[3] * other.m_data[1],
                m_data[0] * other.m_data[3] + m_data[1] * other.m_data[2] - m_data[2] * other.m_data[1] + m_data[3] * other.m_data[0]);
        }
        this_type& operator *= (const this_type& other)
        {
            Copy(*this * other);
            return *this;
        }

        this_type operator - () const
        {
            return this_type(-m_data[0], -m_data[1], -m_data[2], -m_data[3]);
        }

        void GetRotationMatrix3rows(value_type r0[3], value_type r1[3], value_type r2[3]) const
        {
            const value_type ww = Numerics::Sqr(m_data[0]);
            const value_type xx = Numerics::Sqr(m_data[1]);
            const value_type yy = Numerics::Sqr(m_data[2]);
            const value_type zz = Numerics::Sqr(m_data[3]);
            const value_type wx = 2 * m_data[0] * m_data[1];
            const value_type wy = 2 * m_data[0] * m_data[2];
            const value_type wz = 2 * m_data[0] * m_data[3];
            const value_type xy = 2 * m_data[1] * m_data[2];
            const value_type xz = 2 * m_data[1] * m_data[3];
            const value_type yz = 2 * m_data[2] * m_data[3];
            r0[0] = ww + xx - yy - zz;  r0[1] = xy - wz;            r0[2] = xz + wy;
            r1[0] = xy + wz;            r1[1] = ww - xx + yy - zz;  r1[2] = yz - wx;
            r2[0] = xz - wy;            r2[1] = yz + wx;            r2[2] = ww - xx - yy + zz;
        }
        void GetRotationMatrix4rows(value_type r0[4], value_type r1[4], value_type r2[4], value_type r3[4]) const
        {
            GetRotationMatrix3rows(r0, r1, r2);
            r0[3] = 0;  r1[3] = 0;  r2[3] = 0;
            r3[0] = 0;  r3[1] = 0;  r3[2] = 0;  r3[3] = 1;
        }
        void GetRotationMatrix4(value_type r[16]) const
        {
            GetRotationMatrix4rows(&r[0], &r[4], &r[8], &r[12]);
        }
        void GetRotationMatrix3(value_type r[9]) const
        {
            GetRotationMatrix3rows(&r[0], &r[3], &r[6]);
        }

        vector_type Transform(const vector_type& v) const
        {
            //vector_type r0,r1,r2;
            //GetRotationMatrix3rows(r0.GetData(), r1.GetData(), r2.GetData());
            //return vector_type(r0.InnerProduct(v), r1.InnerProduct(v), r2.InnerProduct(v));
            auto res = *this * this_type(0, v[0], v[1], v[2]) * Inverted();
            return vector_type(res.GetX(), res.GetY(), res.GetZ());
        }

        vector_type InverseTransform(const vector_type& v) const
        {
            auto res = Inverted() * this_type(0, v[0], v[1], v[2]) * *this;
            return vector_type(res.GetX(), res.GetY(), res.GetZ());
        }
    };

    template<typename VALUE_TYPE>
    TQuaternion<VALUE_TYPE> Slerp(TQuaternion<VALUE_TYPE> const &q0, TQuaternion<VALUE_TYPE> &q1, VALUE_TYPE t)
    {
        // Already completely rotated?
        if (t >= 1)
        {
            return q1;
        }
        // make the start/end smooth (acceleration)
        t = 0.5*(1 - cos(t*Common::Pi));
        // Calculate angle between them.
        VALUE_TYPE cosHalfTheta = q0.InnerProduct(q1);
        // if q0=q1 or q0=-q1 then theta = 0 and we can return q0
        if (abs(cosHalfTheta) >= 1.0)
        {
            return q0;
        }
        // we want the shortest path
        if (cosHalfTheta < 0)
        {
            cosHalfTheta = -cosHalfTheta;
            q1 = -q1;
        }
        // Calculate temporary values.
        VALUE_TYPE halfTheta = acos(cosHalfTheta);
        VALUE_TYPE sinHalfTheta = sqrt(1.0 - Common::Sqr(cosHalfTheta));
        // if theta = 180 degrees then result is not fully defined
        // we could rotate around any axis normal to q0 or q1
        if (abs(sinHalfTheta) < 0.001)
        {
            // todo: find random orthogonal quat, split rotation in 2 parts: t = 0-0.5, 0.5-1
            return (q0*(1 - t) + q1*t).Normalized();
        }
        VALUE_TYPE ratioA = sin((1 - t) * halfTheta) / sinHalfTheta;
        VALUE_TYPE ratioB = sin(t * halfTheta) / sinHalfTheta;
        //calculate Quaternion.
        return TQuaternion<VALUE_TYPE>((q0[0] * ratioA + q1[0] * ratioB), (q0[1] * ratioA + q1[1] * ratioB), (q0[2] * ratioA + q1[2] * ratioB), (q0[3] * ratioA + q1[3] * ratioB));
    }

    template<typename VALUE_TYPE>
    TQuaternion<VALUE_TYPE> operator * (const VALUE_TYPE& value, const TQuaternion<VALUE_TYPE>& q)
    {
        return q * value;
    }
};

