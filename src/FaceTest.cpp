#include "gtest/gtest.h"
using namespace testing;

#include "Cube.h"
using namespace Geometry;

class FaceTest : public Test 
{
protected:
	virtual void SetUp() 
    {
    }

    virtual void TearDown() 
    {
    }

    int FaceCount(ShapePtr shape)
    {
        int count = 0;
        shape->ForEachFace([&count](const FacePtr& face)
        {
            count++;
        });
        return count;
    };
};

TEST_F(FaceTest, SplitSquare) 
{
    ShapePtr shape = std::make_shared<Cube>();
    FacePtr face = *(*(*shape->GetHulls().begin())->GetPatches().begin())->GetFaces().begin();
    face->CheckPointering();
    EXPECT_EQ(6, FaceCount(shape));
    face->Split();
    EXPECT_EQ(7, FaceCount(shape));
}

TEST_F(FaceTest, SplitTriangle)
{
    ShapePtr shape = std::make_shared<Cube>();
    shape->Triangulate();
    FacePtr face = *(*(*shape->GetHulls().begin())->GetPatches().begin())->GetFaces().begin();
    face->CheckPointering();
    EXPECT_EQ(12, FaceCount(shape));
    face->Split();
    EXPECT_EQ(13, FaceCount(shape));
}
