#pragma once

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


