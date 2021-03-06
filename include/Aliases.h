#pragma once

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
    using VertexRaw = raw_ptr<Vertex>;

    using Normal = Vector3d;
    using NormalPtr = std::shared_ptr<Normal>;
    using NormalRaw = raw_ptr<Normal>;

    template<typename POINT> class TLine;
    using Line2d = TLine<Vector2d>;
    using Line3d = TLine<Vector3d>;
    using Line3dPtr = std::shared_ptr<Line3d>;
    using Line3dRaw = raw_ptr<Line3d>;

    template<typename VALUE_TYPE> class TQuaternion;
    using Quat = TQuaternion<double>;

    class Edge;
    using EdgePtr = std::shared_ptr<Edge>;
    using EdgeRaw = raw_ptr<Edge>;

    class Face;
    using FacePtr = std::shared_ptr<Face>;
    using FaceRaw = raw_ptr<Face>;

    class Hull;
    using HullPtr = std::shared_ptr<Hull>;
    using HullRaw = raw_ptr<Hull>;

    class Shape;
    using ShapePtr = std::shared_ptr<Shape>;
    using ShapeRaw = raw_ptr<Shape>;

    using TextureCoord = Vector2d;
    using TextureCoordPtr = std::shared_ptr<TextureCoord>;
    using TextureCoordRaw = raw_ptr<TextureCoord>;

    using MousePos = Vector2i;

    template<typename VALUE_TYPE> class TRGBAColor;
    using RGBAColord = TRGBAColor<double>;
    using RGBAColorf = TRGBAColor<float>;
    using RGBAColorb = TRGBAColor<unsigned char>;

    template<typename VALUE_TYPE> class TRGBColor;
    using RGBColord = TRGBColor<double>;
    using RGBColorf = TRGBColor<float>;
    using RGBColorb = TRGBColor<unsigned char>;

    using Color = RGBAColorf;
    using ColorPtr = std::shared_ptr<Color>;
    using ColorRaw = raw_ptr<Color>;

    template<typename VECTOR_TYPE> class TBoundingShape;
    using BoundingShape2d = TBoundingShape<Vector2d>;
    using BoundingShape2f = TBoundingShape<Vector2f>;
    using BoundingShape3d = TBoundingShape<Vector3d>;
    using BoundingShape3f = TBoundingShape<Vector3f>;

    template<typename VALUE_TYPE> class TRotationMatrix3;
    using RotationMatrix3d = TRotationMatrix3<double>;

};

