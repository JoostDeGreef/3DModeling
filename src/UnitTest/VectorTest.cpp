#include "gtest.h"
using namespace testing;

#include "Common.h"
using namespace Common;

#include "Geometry.h"
using namespace Geometry;

class VectorTest : public Test 
{
protected:
	virtual void SetUp() 
    {
    }

    virtual void TearDown() 
    {
    }
};

TEST_F(VectorTest, DefaultConstructor) 
{
    Vector3d v3;
    EXPECT_EQ(3, v3.dimension);
    Vector2d v2;
    EXPECT_EQ(2, v2.dimension);
}

TEST_F(VectorTest, InitConstructor) 
{
    Vector3d v3(1,2,3);
    EXPECT_EQ(1,v3[0]);
    EXPECT_EQ(2,v3[1]);
    EXPECT_EQ(3,v3[2]);
    Vector2d v2(1, 2);
    EXPECT_EQ(1, v2[0]);
    EXPECT_EQ(2, v2[1]);
}

TEST_F(VectorTest, CopyConstructor) 
{
    Vector3d v0(1,2,3);
    Vector3d v1(v0);
    EXPECT_EQ(1,v1[0]);
    EXPECT_EQ(2,v1[1]);
    EXPECT_EQ(3,v1[2]);
}

TEST_F(VectorTest, Length) 
{
    Vector3d v(0,3,4);
    EXPECT_EQ(5,v.Length());
}

TEST_F(VectorTest, InnerProduct) 
{
    Vector3d v0(2,3,4);
    Vector3d v1(3,4,5);
    EXPECT_EQ(38,v0.InnerProduct(v1));
}

TEST_F(VectorTest, Add) 
{
    Vector3d v0(0,3,4);
    Vector3d v1(2,1,5);
    Vector3d v2 = v0 + v1;
    EXPECT_EQ(2,v2[0]);
    EXPECT_EQ(4,v2[1]);
    EXPECT_EQ(9,v2[2]);
}

TEST_F(VectorTest, AddSet) 
{
    Vector3d v0(0,3,4);
    Vector3d v1(2,1,5);
    v0 += v1;
    EXPECT_EQ(2,v0[0]);
    EXPECT_EQ(4,v0[1]);
    EXPECT_EQ(9,v0[2]);
}

TEST_F(VectorTest, Subtract) 
{
    Vector3d v0(0,3,4);
    Vector3d v1(2,1,5);
    Vector3d v2 = v0 - v1;
    EXPECT_EQ(-2,v2[0]);
    EXPECT_EQ( 2,v2[1]);
    EXPECT_EQ(-1,v2[2]);
}

TEST_F(VectorTest, SubtractSet) 
{
    Vector3d v0(0,3,4);
    Vector3d v1(2,1,5);
    v0 -= v1;
    EXPECT_EQ(-2,v0[0]);
    EXPECT_EQ( 2,v0[1]);
    EXPECT_EQ(-1,v0[2]);
}

TEST_F(VectorTest, CrossProduct) 
{
    Vector3d v0(2,4,5);
    Vector3d v1(3,2,3);
    Vector3d v2 = CrossProduct(v0,v1);
    EXPECT_EQ( 2,v2[0]);
    EXPECT_EQ( 9,v2[1]);
    EXPECT_EQ(-8,v2[2]);
}

TEST_F(VectorTest, ScalarTripleProduct) 
{
    Vector3d v0(1,2,3);
    Vector3d v1(4,3,2);
    Vector3d v2(4,5,1);
    EXPECT_EQ(25,ScalarTripleProduct(v0,v1,v2));
}

TEST_F(VectorTest, VectorTripleProduct) 
{
    Vector3d v0(1,2,3);
    Vector3d v1(4,3,2);
    Vector3d v2(4,5,1);
    Vector3d v3 = VectorTripleProduct(v0,v1,v2);
    EXPECT_EQ(  4,v3[0]);
    EXPECT_EQ(-29,v3[1]);
    EXPECT_EQ( 18,v3[2]);
}
