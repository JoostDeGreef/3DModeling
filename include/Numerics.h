#pragma once

namespace Geometry
{
    namespace Numerics
    {
        // random number [0.0, max)
        template<typename T> inline T NormalizedRandomNumber(T max = 1)
        {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            static std::uniform_real_distribution<double> dis(0, 1);
            return (T)(max*dis(gen));
        }


        template<typename T> inline T Sqr(const T& value)
        {
            return value*value;
        }

        template<typename T> inline T Clamp(const T& value, const T& minValue, const T& maxValue)
        {
            if (value < minValue)
            {
                return minValue;
            }
            else if (value < maxValue)
            {
                return value;
            }
            else
            {
                return maxValue;
            }
        }

        class Constants
        {
        public:
            static constexpr auto Pi = 3.14159265358979323846;
        };

        template<typename T>
        class Limits
        {
        public:
            static constexpr T CompareEpsilon = (T)1e-10;
            static constexpr T MinValue = std::numeric_limits<T>::lowest();
            static constexpr T MaxValue = std::numeric_limits<T>::max();
        };

        // 
        // true if v0-eps <= v1 <= v0+eps
        //
        template<typename T>
        inline bool Equal(const T &v0, const T &v1, const T &epsilon = Limits<T>::CompareEpsilon)
        {
            return v0 <= v1 + epsilon && v0 + epsilon >= v1;
        }

        // 
        // true if v0>v1+eps || v0+eps<v1
        //
        template<typename T>
        inline bool NotEqual(const T &v0, const T &v1, const T &epsilon = Limits<T>::CompareEpsilon)
        {
            return v0>v1 + epsilon || v0 + epsilon<v1;
        }

        // 
        // true if v0 > v1+eps
        //
        template<typename T>
        inline bool Greater(const T &v0, const T &v1, const T &epsilon = Limits<T>::CompareEpsilon)
        {
            return v0>v1 + epsilon;
        }

        // 
        // true if v0+eps >= v1
        //
        template<typename T>
        inline bool GreaterOrEqual(const T &v0, const T &v1, const T &epsilon = Limits<T>::CompareEpsilon)
        {
            return v0 + epsilon >= v1;
        }

        // 
        // true if v0+eps < v1 
        //
        template<typename T>
        inline bool Less(const T &v0, const T &v1, const T &epsilon = Limits<T>::CompareEpsilon)
        {
            return v0 + epsilon<v1;
        }

        // 
        // true if v0-eps <= v1 
        //
        template<typename T>
        inline bool LessOrEqual(const T &v0, const T &v1, const T &epsilon = Limits<T>::CompareEpsilon)
        {
            return v0 <= v1 + epsilon;
        }

        // -1 : v0+eps < v1 
        //  0 : v0-eps <= v1 <= v0+eps
        //  1 : v0 > v1+eps
        template<typename T>
        inline int Compare(const T &v0, const T &v1, const T &epsilon = Limits<T>::CompareEpsilon)
        {
            return (v0 + epsilon<v1) ? (-1) :
                ((v0>v1 + epsilon) ? (1) :
                (0));
        }

    }
}

