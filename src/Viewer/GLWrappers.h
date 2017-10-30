#pragma once

#include "GLAD/glad.h"
#include "GLFW/glfw3.h"

namespace Viewer
{
    void glColor(const Geometry::RGBAColorf& color);
    void glColor(const Geometry::RGBAColord& color);
    void glColor(const Geometry::RGBColorf& color);
    void glColor(const Geometry::RGBColord& color);
    inline void glColor(const Geometry::ColorPtr& color) { glColor(*color); }

    void glVertex(const Geometry::Vector3f& point);
    void glVertex(const Geometry::Vector3d& point);
    inline void glVertex(const Geometry::VertexPtr& point) { glVertex(*point); }
    void glVertex(const double& x, const double& y);
    void glVertex(const float& x, const float& y);

    void glNormal(const Geometry::Vector3f& normal);
    void glNormal(const Geometry::Vector3d& normal);
    inline void glNormal(const Geometry::NormalPtr& normal) { glNormal(*normal); }

    void glTextureCoord(const Geometry::Vector2f& coord);
    void glTextureCoord(const Geometry::Vector2d& coord);
    inline void glTextureCoord(const Geometry::TextureCoordPtr& coord) { glTextureCoord(*coord); }

    void glMultMatrix(const Geometry::Quat& quat);

    void glLoadMatrix(const Geometry::Quat& quat);
};


