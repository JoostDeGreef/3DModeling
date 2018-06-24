#include "CommonTestFunctionality.h"

class LineTest : public Test
{
protected:
    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }

    void Test(Line2d l0, Line2d l1, Line2d::point_type p, Line2d::Intersection::Type type0, Line2d::Intersection::Type type1, bool isParallel, bool linesIntersect, bool linesSharePoint, bool segmentsIntersect)
    {
        auto intersection = l0.CalculateIntersection(l1);
        EXPECT_EQ(isParallel, intersection.IsParallel());
        EXPECT_EQ(linesIntersect, intersection.LinesIntersect());
        EXPECT_EQ(linesSharePoint, intersection.LinesSharePoint());
        EXPECT_EQ(segmentsIntersect, intersection.SegmentsIntersect());
        EXPECT_EQ(type0, intersection.GetType(0));
        EXPECT_EQ(type1, intersection.GetType(1));
        if(!isParallel) EXPECT_EQ(p, intersection.GetIntersection());
    }
};


TEST_F(LineTest, Parallel)
{
    Test({ { 0,0 },{ 1,0 } }, { { 0,1 },{ 1,1 } }, { 0,0 }, Line2d::Intersection::Parallel, Line2d::Intersection::Parallel, true, false, false, false);
}

TEST_F(LineTest, BeforeBefore)
{
    Test({ { 0,0 },{ 1,0 } }, { { 0,1 },{ 1,2 } }, { -1,0 }, Line2d::Intersection::Before, Line2d::Intersection::Before, false, true, false, false);
}

TEST_F(LineTest, BeforeStart)
{
    Test({ { 0,0 },{ 1,0 } }, { { -1,0 },{ 1,1 } }, { -1,0 }, Line2d::Intersection::Before, Line2d::Intersection::Start, false, true, false, false);
}

TEST_F(LineTest, BeforeOn)
{
    Test({ { 0,0 },{ 1,0 } }, { { 0,1 },{-1,-1 } }, {-0.5,0 }, Line2d::Intersection::Before, Line2d::Intersection::On, false, true, false, false);
}

TEST_F(LineTest, BeforeEnd)
{
    Test({ { 0,0 },{ 1,0 } }, { {-1,1 },{-1,0 } }, {-1,0 }, Line2d::Intersection::Before, Line2d::Intersection::End, false, true, false, false);
}

TEST_F(LineTest, BeforeAfter)
{
    Test({ { 0,0 },{ 1,0 } }, { {-1,2 },{-1,1 } }, {-1,0 }, Line2d::Intersection::Before, Line2d::Intersection::After, false, true, false, false);
}



TEST_F(LineTest, StartBefore)
{
    Test({ { 0,0 },{ 1,0 } }, { { 0,1 },{ 0,2 } }, { 0,0 }, Line2d::Intersection::Start, Line2d::Intersection::Before, false, true, false, false);
}

TEST_F(LineTest, StartStart)
{
    Test({ { 0,0 },{ 1,0 } }, { { 0,0 },{ 1,1 } }, { 0,0 }, Line2d::Intersection::Start, Line2d::Intersection::Start, false, true, true, true);
}

TEST_F(LineTest, StartOn)
{
    Test({ { 0,0 },{ 1,0 } }, { {-1,-1 },{ 1,1 } }, { 0,0 }, Line2d::Intersection::Start, Line2d::Intersection::On, false, true, false, true);
}

TEST_F(LineTest, StartEnd)
{
    Test({ { 0,1 },{ 1,1 } }, { {-1,-1 },{ 0,1 } }, { 0,1 }, Line2d::Intersection::Start, Line2d::Intersection::End, false, true, true, true);
}

TEST_F(LineTest, StartAfter)
{
    Test({ { 0,0 },{ 1,0 } }, { { 0,2 },{ 0,1 } }, { 0,0 }, Line2d::Intersection::Start, Line2d::Intersection::After, false, true, false, false);
}



TEST_F(LineTest, OnBefore)
{
    Test({ {-1,0 },{ 1,0 } }, { { 0,1 },{ 0,2 } }, { 0,0 }, Line2d::Intersection::On, Line2d::Intersection::Before, false, true, false, false);
}

TEST_F(LineTest, OnStart)
{
    Test({ {-1,0 },{ 1,0 } }, { { 0,0 },{ 1,1 } }, { 0,0 }, Line2d::Intersection::On, Line2d::Intersection::Start, false, true, false, true);
}

TEST_F(LineTest, OnOn)
{
    Test({ { -1,0 },{ 1,0 } }, { { 0,1 },{ 0,-1 } }, { 0,0 }, Line2d::Intersection::On, Line2d::Intersection::On, false, true, false, true);
}

TEST_F(LineTest, OnEnd)
{
    Test({ {-1,0 },{ 1,0 } }, { { 0,1 },{ 0,0 } }, { 0,0 }, Line2d::Intersection::On, Line2d::Intersection::End, false, true, false, true);
}

TEST_F(LineTest, OnAfter)
{
    Test({ {-1,0 },{ 1,0 } }, { { 0,2 },{ 0,1 } }, { 0,0 }, Line2d::Intersection::On, Line2d::Intersection::After, false, true, false, false);
}



TEST_F(LineTest, EndBefore)
{
    Test({ {-1,0 },{ 1,0 } }, { { 0,1 },{-1,2 } }, { 1,0 }, Line2d::Intersection::End, Line2d::Intersection::Before, false, true, false, false);
}

TEST_F(LineTest, EndStart)
{
    Test({ { 0,0 },{ 1,0 } }, { { 1,0 },{ 1,1 } }, { 1,0 }, Line2d::Intersection::End, Line2d::Intersection::Start, false, true, true, true);
}

TEST_F(LineTest, EndOn)
{
    Test({ { 0,0 },{ 1,0 } }, { { 1,1 },{ 1,-1 } }, { 1,0 }, Line2d::Intersection::End, Line2d::Intersection::On, false, true, false, true);
}

TEST_F(LineTest, EndEnd)
{
    Test({ { 0,0 },{ 1,0 } }, { { 0,1 },{ 1,0 } }, { 1,0 }, Line2d::Intersection::End, Line2d::Intersection::End, false, true, true, true);
}

TEST_F(LineTest, EndAfter)
{
    Test({ {-1,0 },{ 1,0 } }, { {-1,2 },{ 0,1 } }, { 1,0 }, Line2d::Intersection::End, Line2d::Intersection::After, false, true, false, false);
}



TEST_F(LineTest, AfterBefore)
{
    Test({ { 0,2 },{ 0,1 } }, { { 1,0 },{ 3,0 } }, { 0,0 }, Line2d::Intersection::After, Line2d::Intersection::Before, false, true, false, false);
}

TEST_F(LineTest, AfterStart)
{
    Test({ { 0,3 },{ 0,1 } }, { { 0,0 },{ 1,0 } }, { 0,0 }, Line2d::Intersection::After, Line2d::Intersection::Start, false, true, false, false);
}

TEST_F(LineTest, AfterOn)
{
    Test({ { 0,3 },{ 0,1 } }, { { -1,0 },{ 1,0 } }, { 0,0 }, Line2d::Intersection::After, Line2d::Intersection::On, false, true, false, false);
}

TEST_F(LineTest, AfterEnd)
{
    Test({ { 0,3 },{ 0,1 } }, { { 1,0 },{ 0,0 } }, { 0,0 }, Line2d::Intersection::After, Line2d::Intersection::End, false, true, false, false);
}

TEST_F(LineTest, AfterAfter)
{
    Test({ { 0,3 },{ 0,1 } }, { { 2,0 },{ 1,0 } }, { 0,0 }, Line2d::Intersection::After, Line2d::Intersection::After, false, true, false, false);
}


