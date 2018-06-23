#include "CommonTestFunctionality.h"

class ContourTest : public Test 
{
protected:
	virtual void SetUp() 
    {
    }

    virtual void TearDown() 
    {
    }
};

TEST_F(ContourTest, SingleLoop)
{
    Contour contour;
    auto points = contour.GetPoints();
    EXPECT_EQ(0, points.size());
    Contour contour2({Vector2d(0,0), Vector2d(0,1), Vector2d(1,1)});
    contour = contour2;
    points = contour.GetPoints();
    EXPECT_EQ(3, points.size());
    contour.Add(Vector2d(1, 0));
    points = contour.GetPoints();
    EXPECT_EQ(4, points.size());
}

TEST_F(ContourTest, ChangeOrientation)
{
    Contour contour({ Vector2d(0,0), Vector2d(0,1), Vector2d(1,1), Vector2d(1,0) });
    contour.ForceClockwise();
    auto points = contour.GetPoints();
    ASSERT_EQ(4, points.size());
    EXPECT_EQ(Vector2d(0, 0), points[0]);
    EXPECT_EQ(Vector2d(0, 1), points[1]);
    EXPECT_EQ(Vector2d(1, 1), points[2]);
    EXPECT_EQ(Vector2d(1, 0), points[3]);
    contour = Contour({ Vector2d(0,0), Vector2d(1,0), Vector2d(1,1), Vector2d(0,1) });
    contour.ForceClockwise();
    points = contour.GetPoints();
    ASSERT_EQ(4, points.size());
    EXPECT_EQ(Vector2d(0, 1), points[0]);
    EXPECT_EQ(Vector2d(1, 1), points[1]);
    EXPECT_EQ(Vector2d(1, 0), points[2]);
    EXPECT_EQ(Vector2d(0, 0), points[3]);
}


