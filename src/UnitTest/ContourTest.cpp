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
    auto loops = contour.GetClockwiseLoops();
    EXPECT_EQ(0, loops.size());
    Contour contour2({Vector2d(0,0), Vector2d(0,1), Vector2d(1,1)});
    contour = contour2;
    loops = contour.GetClockwiseLoops();
    ASSERT_EQ(1, loops.size());
    EXPECT_EQ(3, loops.front().size());
    contour.Add(Vector2d(1, 0));
    loops = contour.GetClockwiseLoops();
    ASSERT_EQ(1, loops.size());
    EXPECT_EQ(4, loops.front().size());
}

TEST_F(ContourTest, ChangeOrientation)
{
    Contour contour({ Vector2d(0,0), Vector2d(0,1), Vector2d(1,1), Vector2d(1,0) });
    auto loops = contour.GetClockwiseLoops();
    ASSERT_EQ(1, loops.size());
    EXPECT_EQ(4, loops.front().size());
    EXPECT_EQ(Vector2d(0, 0), loops[0][0]);
    EXPECT_EQ(Vector2d(0, 1), loops[0][1]);
    EXPECT_EQ(Vector2d(1, 1), loops[0][2]);
    EXPECT_EQ(Vector2d(1, 0), loops[0][3]);
    contour = Contour({ Vector2d(0,0), Vector2d(1,0), Vector2d(1,1), Vector2d(0,1) });
    loops = contour.GetClockwiseLoops();
    ASSERT_EQ(1, loops.size());
    EXPECT_EQ(4, loops.front().size());
    EXPECT_EQ(Vector2d(0, 1), loops[0][0]);
    EXPECT_EQ(Vector2d(1, 1), loops[0][1]);
    EXPECT_EQ(Vector2d(1, 0), loops[0][2]);
    EXPECT_EQ(Vector2d(0, 0), loops[0][3]);
}

TEST_F(ContourTest, MultipleLoops)
{
    Contour contour;
    auto loops = contour.GetClockwiseLoops();
    EXPECT_EQ(0, loops.size());
    contour = Contour({ Vector2d(0,0), Vector2d(1,0), Vector2d(1,1) });
    loops = contour.GetClockwiseLoops();
    ASSERT_EQ(1, loops.size());
    EXPECT_EQ(3, loops.front().size());
    contour.Add({ Vector2d(-1, 1), Vector2d(-1, 2), Vector2d(0, 2) });
    loops = contour.GetClockwiseLoops();
    ASSERT_EQ(2, loops.size());
    EXPECT_EQ(4, loops.front().size());
    EXPECT_EQ(4, loops.back().size());
}

