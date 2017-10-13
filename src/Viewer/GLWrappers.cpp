#include "Geometry.h"
#include "GLWrappers.h"
using namespace Geometry;

#include "GLAD/glad.h"
#include "GLFW/glfw3.h"

namespace Viewer
{

void glColor(const RGBAColorf & color)
{
    glColor4fv(color.GetRGBA());
}
void glColor(const RGBAColord & color)
{
    glColor4dv(color.GetRGBA());
}

void glColor(const RGBColorf & color)
{
    glColor3fv(color.GetRGB());
}
void glColor(const RGBColord & color)
{
    glColor3dv(color.GetRGB());
}

void glVertex(const Vector3f & point)
{
    glVertex3fv(point.GetData());
}
void glVertex(const Vector3d & point)
{
    glVertex3dv(point.GetData());
}

void glVertex(const double& x, const double& y)
{
    glVertex2d(x,y);
}
void glVertex(const float& x, const float& y)
{
    glVertex2f(x,y);
}

void glNormal(const Vector3f & normal)
{
    glNormal3fv(normal.GetData());
}

void glNormal(const Vector3d & normal)
{
    glNormal3dv(normal.GetData());
}

void glTextureCoord(const Vector2f& coord)
{
    glTexCoord2fv(coord.GetData());
}

void glTextureCoord(const Vector2d& coord)
{
    glTexCoord2dv(coord.GetData());
}

void glMultMatrix(const Quat& quat)
{
    double r[16];
    quat.GetRotationMatrix4(r);
    glMultMatrixd(r);
}

void glLoadMatrix(const Quat& quat)
{
    double r[16];
    quat.GetRotationMatrix4(r);
    glLoadMatrixd(r);
}

} // namespace Viewer

