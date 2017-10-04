#include <memory>
using namespace std;

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

TEST_F(FaceTest, CalcNormal)
{
    Shape shape;
    HullPtr hull = shape.ConstructAndAddHull();
    PatchPtr patch = hull->ConstructAndAddPatch();
    FacePtr face = patch->ConstructAndAddFace();
    auto e0 = Face::ConstructAndAddEdge(face, make_shared<Vector3d>(0, 0, 0));
    auto e1 = Face::ConstructAndAddEdge(face, make_shared<Vector3d>(1, 0, 0));
    auto e2 = Face::ConstructAndAddEdge(face, make_shared<Vector3d>(0, 1, 0));
    e0->SetNext(e1);
    e1->SetNext(e2);
    e2->SetNext(e0);
    e0->SetPrev(e2);
    e1->SetPrev(e0);
    e2->SetPrev(e1);
    e0->SetTwin(e0);
    e1->SetTwin(e1);
    e2->SetTwin(e2);
    face->CalcNormal();
    EXPECT_EQ(Vector3d(0,0,1),*face->GetNormal());
}

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
