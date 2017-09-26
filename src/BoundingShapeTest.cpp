#include "gtest/gtest.h"
using namespace testing;

#include "Aliases.h"
#include "BoundingShape.h"
using namespace Geometry;

class BoundingShapeTest : public Test 
{
protected:
	virtual void SetUp() 
    {
    }

    virtual void TearDown() 
    {
    }
};

TEST_F(BoundingShapeTest, DefaultConstructor)
{
    BoundingShape3d box3;
    EXPECT_EQ(3, box3.dimension);
    EXPECT_FALSE(box3.IsInitialized());
    BoundingShape2d box2;
    EXPECT_EQ(2, box2.dimension);
    EXPECT_FALSE(box2.IsInitialized());
}

TEST_F(BoundingShapeTest, Encapsulates)
{
    BoundingShape3d box3(Vector3d(0,0,0),Vector3d(1,1,1));
    EXPECT_TRUE(box3.IsInitialized());
    EXPECT_TRUE(box3.Encapsulates(Vector3d(0.5, 0.5, 0.5)));
    EXPECT_FALSE(box3.Encapsulates(Vector3d(0.5, 0.5, 1.5)));
    BoundingShape3d ball3(Vector3d(0, 0, 0), 1);
    EXPECT_TRUE(ball3.IsInitialized());
    EXPECT_TRUE(ball3.Encapsulates(Vector3d(0.5, 0.5, 0.5)));
    EXPECT_FALSE(ball3.Encapsulates(Vector3d(0.5, 0.5, 1.5)));
}

TEST_F(BoundingShapeTest, Convert)
{
    BoundingShape3d box3(Vector3d(0, 0, 0), Vector3d(2, 2, 2));
    BoundingShape3d ball3 = box3;
    ball3.Convert(BoundingShape3d::Type::Ball);
    EXPECT_DOUBLE_EQ(1, ball3.GetCenter()[0]);
    EXPECT_TRUE(box3.IsOptimal());
    EXPECT_FALSE(ball3.IsOptimal());
}

TEST_F(BoundingShapeTest, BallTouchesBox)
{
    //  0 | 1 | 2
    //  --+---+--
    //  3 | 4 | 5
    //  --+---+--
    //  6 | 7 | 8
    BoundingShape3d box(Vector3d(-2, -2, -2), Vector3d(2, 2, 2));

    BoundingShape3d ball00(Vector3d(0, -3, -3), 1);
    BoundingShape3d ball01(Vector3d(0, -3, -3), 2);
    EXPECT_FALSE(box.Touches(ball00));
    EXPECT_TRUE(box.Touches(ball01));
    BoundingShape3d ball10(Vector3d(0,  0, -3), 0.5);
    BoundingShape3d ball11(Vector3d(0,  0, -3), 1);
    BoundingShape3d ball12(Vector3d(0,  0, -3), 2);
    EXPECT_FALSE(box.Touches(ball10));
    EXPECT_TRUE(box.Touches(ball11));
    EXPECT_TRUE(box.Touches(ball12));
    BoundingShape3d ball20(Vector3d(0, 3, -3), 1);
    BoundingShape3d ball21(Vector3d(0, 3, -3), 2);
    EXPECT_FALSE(box.Touches(ball20));
    EXPECT_TRUE(box.Touches(ball21));

    BoundingShape3d ball30(Vector3d(0, -3, 0), 0.5);
    BoundingShape3d ball31(Vector3d(0, -3, 0), 1);
    BoundingShape3d ball32(Vector3d(0, -3, 0), 2);
    EXPECT_FALSE(box.Touches(ball30));
    EXPECT_TRUE(box.Touches(ball31));
    EXPECT_TRUE(box.Touches(ball32));
    BoundingShape3d ball40(Vector3d(0, 0, 0), 1);
    BoundingShape3d ball41(Vector3d(0, 0, 0), 2);
    BoundingShape3d ball42(Vector3d(0, 0, 0), 3);
    BoundingShape3d ball43(Vector3d(0, 0, 0), 6);
    EXPECT_TRUE(box.Touches(ball40));
    EXPECT_TRUE(box.Touches(ball41));
    EXPECT_TRUE(box.Touches(ball42));
    EXPECT_TRUE(box.Touches(ball43));
    BoundingShape3d ball50(Vector3d(0, 3, 0), 0.5);
    BoundingShape3d ball51(Vector3d(0, 3, 0), 1);
    BoundingShape3d ball52(Vector3d(0, 3, 0), 2);
    EXPECT_FALSE(box.Touches(ball50));
    EXPECT_TRUE(box.Touches(ball51));
    EXPECT_TRUE(box.Touches(ball52));

    BoundingShape3d ball60(Vector3d(0, -3, 3), 1);
    BoundingShape3d ball61(Vector3d(0, -3, 3), 2);
    EXPECT_FALSE(box.Touches(ball60));
    EXPECT_TRUE(box.Touches(ball61));
    BoundingShape3d ball70(Vector3d(0, 0, 3), 0.5);
    BoundingShape3d ball71(Vector3d(0, 0, 3), 1);
    BoundingShape3d ball72(Vector3d(0, 0, 3), 2);
    EXPECT_FALSE(box.Touches(ball70));
    EXPECT_TRUE(box.Touches(ball71));
    EXPECT_TRUE(box.Touches(ball72));
    BoundingShape3d ball80(Vector3d(0, 3, 3), 1);
    BoundingShape3d ball81(Vector3d(0, 3, 3), 2);
    EXPECT_FALSE(box.Touches(ball80));
    EXPECT_TRUE(box.Touches(ball81));
}
