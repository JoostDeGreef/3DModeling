#ifndef GEOMETRY_ALIASES_H
#define GEOMETRY_ALIASES_H 1

#include <memory>

namespace Geometry
{
    template<typename VALUE_TYPE, unsigned int DIMENSION> class TVector;

    template<typename VALUE_TYPE> 
    using TVector3 = TVector<VALUE_TYPE, 3>;
    using Vector3d = TVector3<double>;
    using Vector3f = TVector3<float>;

    template<typename VALUE_TYPE>
    using TVector2 = TVector<VALUE_TYPE, 2>;
    using Vector2d = TVector2<double>;
    using Vector2f = TVector2<float>;
    using Vector2i = TVector2<int>;

    using Vertex = Vector3d;
    using VertexPtr = std::shared_ptr<Vertex>;

    template<typename POINT_TYPE> class TLine;
    using Line2d = TLine<Vector2d>;
    using Line3d = TLine<Vector3d>;

    using Normal = Vector3d;
    using NormalPtr = std::shared_ptr<Normal>;

    class Edge;
    using EdgePtr = std::shared_ptr<Edge>;

    class Face;
    using FacePtr = std::shared_ptr<Face>;

    class Patch;
    using PatchPtr = std::shared_ptr<Patch>;

    class Hull;
    using HullPtr = std::shared_ptr<Hull>;

    class Shape;
    using ShapePtr = std::shared_ptr<Shape>;

    using TextureCoord = Vector2d;
    using TextureCoordPtr = std::shared_ptr<TextureCoord>;

    using MousePos = Vector2i;

    template<typename VALUE_TYPE> class TQuaternion;
    using Quat = TQuaternion<double>;

    template<typename VALUE_TYPE> class TRGBColor;
    using RGBColord = TRGBColor<double>;
    using RGBColorf = TRGBColor<float>;
    using RGBColorb = TRGBColor<unsigned char>;

    template<typename VALUE_TYPE> class TRGBAColor;
    using RGBAColord = TRGBAColor<double>;
    using RGBAColorf = TRGBAColor<float>;
    using RGBAColorb = TRGBAColor<unsigned char>;

    using Color = RGBAColorf;
    using ColorPtr = std::shared_ptr<Color>;

    template<typename VECTOR_TYPE> class TBoundingShape;
    using BoundingShape2d = TBoundingShape<Vector2d>;
    using BoundingShape2f = TBoundingShape<Vector2f>;
    using BoundingShape3d = TBoundingShape<Vector3d>;
    using BoundingShape3f = TBoundingShape<Vector3f>;

    template<typename VALUE_TYPE> class TRotationMatrix3;
    using RotationMatrix3d = TRotationMatrix3<double>;

};

#endif // GEOMETRY_ALIASES_H
