#ifndef GEOMETRY_RENDEROBJECT_H
#define GEOMETRY_RENDEROBJECT_H 1

namespace Geometry
{
    class IRenderObject
    {
    public:
        IRenderObject() {}
        virtual ~IRenderObject() {}
        virtual void Invalidate() = 0;
    };

    class NOPRenderObject : public IRenderObject
    {
    public:
        NOPRenderObject() {}
        virtual ~NOPRenderObject() {}
        virtual void Invalidate() override {}
    };
}; // Geometry

#endif // GEOMETRY_RENDEROBJECT_H

