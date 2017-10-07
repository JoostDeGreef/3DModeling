#ifndef GEOMETRY_RGBACOLOR_H
#define GEOMETRY_RGBACOLOR_H 1

#include <memory>

#include "Aliases.h"

namespace Geometry
{
    template<typename VALUE_TYPE>
    class TRGBAColor 
    {
    public:
        typedef TRGBAColor<VALUE_TYPE> this_type;
        typedef VALUE_TYPE value_type;

    private:
        value_type m_data[4];

    public:
        TRGBAColor()
        {}
        TRGBAColor(const this_type &other)
        {
            Copy(other);
        }
        TRGBAColor(const value_type &r, const value_type &g, const value_type &b, const value_type &a)
        {
            Set(r, g, b, a);
        }
        TRGBAColor(const unsigned int color)
        {
            SetInt(color);
        }
        template<typename... Args>
        static std::shared_ptr<this_type> Construct(Args&... args)
        {
            SmallObjectAllocator<this_type> allocator;
            return std::allocate_shared<this_type>(allocator, args...);
        }

        void SetR(const value_type &r) { m_data[0] = r; }
        void SetG(const value_type &g) { m_data[1] = g; }
        void SetB(const value_type &b) { m_data[2] = b; }
        void SetA(const value_type &a) { m_data[3] = a; }
        void Set(const value_type &r, const value_type &g, const value_type &b, const value_type &a)
        {
            m_data[0] = r;
            m_data[1] = g;
            m_data[2] = b;
            m_data[3] = a;
        }

        void Copy(const this_type &other)
        {
            m_data[0] = other.m_data[0];
            m_data[1] = other.m_data[1];
            m_data[2] = other.m_data[2];
            m_data[3] = other.m_data[3];
        }

        this_type Mix(const this_type & other)
        {
            return this_type(
                (value_type)0.5*(m_data[0] + other.m_data[0]),
                (value_type)0.5*(m_data[1] + other.m_data[1]),
                (value_type)0.5*(m_data[2] + other.m_data[2]),
                (value_type)0.5*(m_data[3] + other.m_data[3]));
        }

        const value_type& GetR() const { return m_data[0]; }
        const value_type& GetG() const { return m_data[1]; }
        const value_type& GetB() const { return m_data[2]; }
        const value_type& GetA() const { return m_data[3]; }
        const value_type* GetRGBA() const { return m_data; }

        const unsigned int GetInt() const
        {
            return (Value::ToByte(m_data[0]) << 24)
                 + (Value::ToByte(m_data[1]) << 16)
                 + (Value::ToByte(m_data[2]) << 8)
                 + (Value::ToByte(m_data[3]));
        }
        void SetInt(unsigned int color)
        {
            m_data[0] = Value::FromByte((color >> 24) & 255);
            m_data[1] = Value::FromByte((color >> 16) & 255);
            m_data[2] = Value::FromByte((color >>  8) & 255);
            m_data[3] = Value::FromByte((color      ) & 255);
        }

        bool operator == (const this_type &other) const
        {
            return
                m_data[0] == other.m_data[0] &&
                m_data[1] == other.m_data[1] &&
                m_data[2] == other.m_data[2] &&
                m_data[3] == other.m_data[3];
        }

        static           this_type Random() { return this_type(Value::Rnd(), Value::Rnd(), Value::Rnd(), Value::Hi()); }
        static constexpr this_type Black()  { return this_type(Value::Lo(), Value::Lo(), Value::Lo(), Value::Hi()); }
        static constexpr this_type Blue()   { return this_type(Value::Lo(), Value::Lo(), Value::Hi(), Value::Hi()); }
        static constexpr this_type Green()  { return this_type(Value::Lo(), Value::Hi(), Value::Lo(), Value::Hi()); }
        static constexpr this_type Red()    { return this_type(Value::Hi(), Value::Lo(), Value::Lo(), Value::Hi()); }
        static constexpr this_type White()  { return this_type(Value::Hi(), Value::Hi(), Value::Hi(), Value::Hi()); }
    private:

        template <typename T>
        class TValue
        {
        public:
            static constexpr T Hi() { static_assert(false, "Type not supported"); }
            static constexpr T Lo() { static_assert(false, "Type not supported"); }
            static           T Rnd() { static_assert(false, "Type not supported"); }
            static unsigned int ToByte(const T &value) { static_assert(false, "Type not supported"); }
            static           T FromByte(const unsigned int value) { static_assert(false, "Type not supported"); }
        };
        template <>
        class TValue<float>
        {
        public:
            static constexpr float Hi() { return 1.0f; }
            static constexpr float Lo() { return 0.0f; }
            static           float Rnd() { Common::NormalizedRandomNumber<float>(); }
            static unsigned int ToByte(const float &value) { return (unsigned int)(value * 255); }
            static           float FromByte(const unsigned int value) 
            { 
                return value / 255.0f; 
            }
        };
        template <>
        class TValue<double>
        {
        public:
            static constexpr double Hi() { return 1.0; }
            static constexpr double Lo() { return 0.0; }
            static           double Rnd() { Common::NormalizedRandomNumber<double>(); }
            static unsigned int ToByte(const double &value) { return (unsigned int)(value * 255); }
            static           double FromByte(const unsigned int value) { return value / 255.0; }
        };
        template <>
        class TValue<unsigned char>
        {
        public:
            static constexpr unsigned char Hi() { return 255; }
            static constexpr unsigned char Lo() { return 0; }
            static           unsigned char Rnd() { (unsigned char)(255 * Common::NormalizedRandomNumber<double>()); }
            static unsigned int ToByte(const unsigned int &value) { return value; }
            static           unsigned char FromByte(const unsigned int value) { return value; }
        };
        using Value = TValue<value_type>;

    };
}

#endif // GEOMETRY_RGBACOLOR_H
