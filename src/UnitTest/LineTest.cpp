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

    void Test(Line2d l0, Line2d l1, Line2d::point_type p, Line2d::Intersection::Type type, bool isParallel, bool linesIntersect, bool linesSharePoint, bool segmentsIntersect)
    {
        auto intersection = l0.CalculateIntersection(l1);
        EXPECT_EQ(isParallel, intersection.IsParallel());
        EXPECT_EQ(linesIntersect, intersection.LinesIntersect());
        EXPECT_EQ(linesSharePoint, intersection.LinesSharePoint());
        EXPECT_EQ(segmentsIntersect, intersection.SegmentsIntersect());
        EXPECT_EQ(type, intersection.GetType());
        if(!isParallel) EXPECT_EQ(p, intersection.GetIntersection());
    }
};


TEST_F(LineTest, Parallel)
{
    Test({ { 0,0 },{ 1,0 } }, { { 0,1 },{ 1,1 } }, { 0,0 }, Line2d::Intersection::Parallel, true, false, false, false);
}

TEST_F(LineTest, BeforeBefore)
{
    Test({ { 0,0 },{ 1,0 } }, { { 0,1 },{ 1,2 } }, { -1,0 }, Line2d::Intersection::BeforeBefore, false, true, false, false);
}

TEST_F(LineTest, BeforeStart)
{
    Test({ { 0,0 },{ 1,0 } }, { { -1,0 },{ 1,1 } }, { -1,0 }, Line2d::Intersection::BeforeStart, false, true, false, false);
}

TEST_F(LineTest, BeforeOn)
{
    Test({ { 0,0 },{ 1,0 } }, { { 0,1 },{-1,-1 } }, {-0.5,0 }, Line2d::Intersection::BeforeOn, false, true, false, false);
}

TEST_F(LineTest, BeforeEnd)
{
    Test({ { 0,0 },{ 1,0 } }, { {-1,1 },{-1,0 } }, {-1,0 }, Line2d::Intersection::BeforeEnd, false, true, false, false);
}

TEST_F(LineTest, BeforeAfter)
{
    Test({ { 0,0 },{ 1,0 } }, { {-1,2 },{-1,1 } }, {-1,0 }, Line2d::Intersection::BeforeAfter, false, true, false, false);
}



TEST_F(LineTest, StartBefore)
{
    Test({ { 0,0 },{ 1,0 } }, { { 0,1 },{ 0,2 } }, { 0,0 }, Line2d::Intersection::StartBefore, false, true, false, false);
}

TEST_F(LineTest, StartStart)
{
    Test({ { 0,0 },{ 1,0 } }, { { 0,0 },{ 1,1 } }, { 0,0 }, Line2d::Intersection::StartStart, false, true, true, true);
}

TEST_F(LineTest, StartOn)
{
    Test({ { 0,0 },{ 1,0 } }, { {-1,-1 },{ 1,1 } }, { 0,0 }, Line2d::Intersection::StartOn, false, true, false, true);
}

TEST_F(LineTest, StartEnd)
{
    Test({ { 0,1 },{ 1,1 } }, { {-1,-1 },{ 0,1 } }, { 0,1 }, Line2d::Intersection::StartEnd, false, true, true, true);
}

TEST_F(LineTest, StartAfter)
{
    Test({ { 0,0 },{ 1,0 } }, { { 0,2 },{ 0,1 } }, { 0,0 }, Line2d::Intersection::StartAfter, false, true, false, false);
}



TEST_F(LineTest, OnBefore)
{
    Test({ {-1,0 },{ 1,0 } }, { { 0,1 },{ 0,2 } }, { 0,0 }, Line2d::Intersection::OnBefore, false, true, false, false);
}

TEST_F(LineTest, OnStart)
{
    Test({ {-1,0 },{ 1,0 } }, { { 0,0 },{ 1,1 } }, { 0,0 }, Line2d::Intersection::OnStart, false, true, false, true);
}

TEST_F(LineTest, OnOn)
{
    Test({ { -1,0 },{ 1,0 } }, { { 0,1 },{ 0,-1 } }, { 0,0 }, Line2d::Intersection::OnOn, false, true, false, true);
}

TEST_F(LineTest, OnEnd)
{
    Test({ {-1,0 },{ 1,0 } }, { { 0,1 },{ 0,0 } }, { 0,0 }, Line2d::Intersection::OnEnd, false, true, false, true);
}

TEST_F(LineTest, OnAfter)
{
    Test({ {-1,0 },{ 1,0 } }, { { 0,2 },{ 0,1 } }, { 0,0 }, Line2d::Intersection::OnAfter, false, true, false, false);
}



TEST_F(LineTest, EndBefore)
{
    Test({ {-1,0 },{ 1,0 } }, { { 0,1 },{-1,2 } }, { 1,0 }, Line2d::Intersection::EndBefore, false, true, false, false);
}

TEST_F(LineTest, EndStart)
{
    Test({ { 0,0 },{ 1,0 } }, { { 1,0 },{ 1,1 } }, { 1,0 }, Line2d::Intersection::EndStart, false, true, true, true);
}

TEST_F(LineTest, EndOn)
{
    Test({ { 0,0 },{ 1,0 } }, { { 1,1 },{ 1,-1 } }, { 1,0 }, Line2d::Intersection::EndOn, false, true, false, true);
}

TEST_F(LineTest, EndEnd)
{
    Test({ { 0,0 },{ 1,0 } }, { { 0,1 },{ 1,0 } }, { 1,0 }, Line2d::Intersection::EndEnd, false, true, true, true);
}

TEST_F(LineTest, EndAfter)
{
    Test({ {-1,0 },{ 1,0 } }, { {-1,2 },{ 0,1 } }, { 1,0 }, Line2d::Intersection::EndAfter, false, true, false, false);
}



TEST_F(LineTest, AfterBefore)
{
    Test({ { 0,2 },{ 0,1 } }, { { 1,0 },{ 3,0 } }, { 0,0 }, Line2d::Intersection::AfterBefore, false, true, false, false);
}

TEST_F(LineTest, AfterStart)
{
    Test({ { 0,3 },{ 0,1 } }, { { 0,0 },{ 1,0 } }, { 0,0 }, Line2d::Intersection::AfterStart, false, true, false, false);
}

TEST_F(LineTest, AfterOn)
{
    Test({ { 0,3 },{ 0,1 } }, { { -1,0 },{ 1,0 } }, { 0,0 }, Line2d::Intersection::AfterOn, false, true, false, false);
}

TEST_F(LineTest, AfterEnd)
{
    Test({ { 0,3 },{ 0,1 } }, { { 1,0 },{ 0,0 } }, { 0,0 }, Line2d::Intersection::AfterEnd, false, true, false, false);
}

TEST_F(LineTest, AfterAfter)
{
    Test({ { 0,3 },{ 0,1 } }, { { 2,0 },{ 1,0 } }, { 0,0 }, Line2d::Intersection::AfterAfter, false, true, false, false);
}


