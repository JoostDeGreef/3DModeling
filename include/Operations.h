#ifndef GEOMETRY_OPERATIONS_H
#define GEOMETRY_OPERATIONS_H 1

namespace Geometry
{
    // see http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html
    template<typename VALUE_TYPE>
    inline VALUE_TYPE DistancePointToLine(const TVector<VALUE_TYPE, 3>& point, const TVector<VALUE_TYPE, 3>& lineA, const TVector<VALUE_TYPE, 3>& lineB, const VALUE_TYPE& lineLength)
    {
        return CrossProduct(point - lineA, point - lineB).Length() / lineLength;
    }
    template<typename VALUE_TYPE>
    inline VALUE_TYPE DistancePointToLine(const TVector<VALUE_TYPE, 3>& point, const TVector<VALUE_TYPE, 3>& lineA, const TVector<VALUE_TYPE, 3>& lineB)
    {
        return DistancePointToLine(point, lineA, lineB, Distance(lineA, lineB));
    }

    template<typename VALUE_TYPE, unsigned int DIMENSION>
    inline VALUE_TYPE Distance(const TVector<VALUE_TYPE, DIMENSION>& A, const TVector<VALUE_TYPE, DIMENSION>& B)
    {
        return (A - B).Length();
    }
    template<typename VALUE_TYPE, unsigned int DIMENSION>
    inline VALUE_TYPE Distance(const TVector<VALUE_TYPE, DIMENSION>* A, const TVector<VALUE_TYPE, DIMENSION>* B)
    {
        return Distance(*A, *B);
    }

    template<typename VALUE_TYPE, unsigned int DIMENSION>
    inline VALUE_TYPE DistanceSquared(const TVector<VALUE_TYPE, DIMENSION>& A, const TVector<VALUE_TYPE, DIMENSION>& B)
    {
        return (A - B).LengthSquared();
    }
    template<typename VALUE_TYPE, unsigned int DIMENSION>
    inline VALUE_TYPE DistanceSquared(const TVector<VALUE_TYPE, DIMENSION>* A, const TVector<VALUE_TYPE, DIMENSION>* B)
    {
        return DistanceSquared(*A, *B);
    }

    template<typename VALUE_TYPE, unsigned int DIMENSION>
    inline TVector<VALUE_TYPE, DIMENSION> Middle(const TVector<VALUE_TYPE, DIMENSION>& A, const TVector<VALUE_TYPE, DIMENSION>& B)
    {
        return (A + B)/2;
    }
    template<typename VALUE_TYPE, unsigned int DIMENSION>
    inline TVector<VALUE_TYPE, DIMENSION> Middle(const TVector<VALUE_TYPE, DIMENSION>* A, const TVector<VALUE_TYPE, DIMENSION>* B)
    {
        return Middle(*A, *B);
    }
}

#endif // GEOMETRY_OPERATIONS_H
