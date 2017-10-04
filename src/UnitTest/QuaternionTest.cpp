#include "gtest/gtest.h"
using namespace testing;

#include "Aliases.h"
#include "Numerics.h"
#include "Vector.h"
#include "Quaternion.h"
using namespace Geometry;
using namespace Geometry::Numerics;

class QuaternionTest : public Test
{
protected:
    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }

    Vector3d Rotate(const Vector3d& v, const Quat& q)
    {
        double r[16];
        q.GetRotationMatrix4(r);
        Vector3d res(v[0]*r[0] + v[1]*r[1] + v[2]*r[2],
                     v[0]*r[4] + v[1]*r[5] + v[2]*r[6], 
                     v[0]*r[8] + v[1]*r[9] + v[2]*r[10]);
        return res;
    }

    const double eps = 0.00000001;

};

TEST_F(QuaternionTest, DefaultConstructor)
{
    Quat q;
    EXPECT_EQ(Quat(1, 0, 0, 0), q);
}

TEST_F(QuaternionTest, RotationConstructor)
{
    Vector3d axis(1, 1, 0);
    axis.Normalize();
    Quat q(axis, 0);
    EXPECT_EQ(Quat(1, 0, 0, 0), q);
}

TEST_F(QuaternionTest, RotationMatrix)
{
    {
        Vector3d axis(0, 1, 0);
        axis.Normalize();
        Quat q(axis, Constants::Pi);
        Vector3d res = Rotate(Vector3d(1, 0, 0), q);
        EXPECT_EQ(Vector3d(-1,0,0), res);
    }
    {
        Vector3d axis(1, 1, 0);
        axis.Normalize();
        Quat q(axis, Constants::Pi);
        Vector3d res = Rotate(Vector3d(1, 0, 0), q);
        EXPECT_EQ(Vector3d(0, 1, 0), res);
    }
    {
        Vector3d axis(1, 12, 43);
        axis.Normalize();
        Quat q(axis, Constants::Pi*8);
        Vector3d res = Rotate(Vector3d(1, 0, 0), q);
        EXPECT_EQ(Vector3d(1, 0, 0), res);
    }
}

TEST_F(QuaternionTest, Transform)
{
    {
        Vector3d axis(1, 0, 0);
        axis.Normalize();
        Quat q(axis, 0);
        Vector3d v(0, 1, 0);
        v = q.Transform(v);
        EXPECT_EQ(Vector3d(0, 1, 0), v);
    }
    {
        Vector3d axis(1, 0, 0);
        axis.Normalize();
        Quat q(axis, Constants::Pi);
        Vector3d v(0, 1, 0);
        v = q.Transform(v);
        EXPECT_EQ(Vector3d(0, -1, 0), v);
    }
    {
        Vector3d axis(1, 1, 0);
        axis.Normalize();
        Quat q(axis, Constants::Pi);
        Vector3d v(1, 0, 0);
        v = q.Transform(v);
        EXPECT_EQ(Vector3d(0, 1, 0), v);
    }
}


