#include "CommonTestFunctionality.h"

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
        shape->ForEachFace([&count](const FaceRaw& face)
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
    FacePtr face = hull->ConstructAndAddFace();
    auto e0 = face->ConstructAndAddEdge(Construct<Vertex>(0, 0, 0));
    auto e1 = face->ConstructAndAddEdge(Construct<Vertex>(1, 0, 0));
    auto e2 = face->ConstructAndAddEdge(Construct<Vertex>(0, 1, 0));
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
    ShapePtr shape = Construct<Cube>();
    FacePtr face = *(*shape->GetHulls().begin())->GetFaces().begin();
    face->CheckPointering();
    EXPECT_EQ(6, FaceCount(shape));
    face->Split();
    EXPECT_EQ(7, FaceCount(shape));
}

TEST_F(FaceTest, SplitTriangle)
{
    ShapePtr shape = Construct<Cube>();
    shape->Triangulate();
    FacePtr face = *(*shape->GetHulls().begin())->GetFaces().begin();
    face->CheckPointering();
    EXPECT_EQ(12, FaceCount(shape));
    face->Split();
    EXPECT_EQ(13, FaceCount(shape));
}

TEST_F(FaceTest, GetContourLineIntersections)
{
    ShapePtr shape = Construct<Cube>();
    FacePtr face = *(*shape->GetHulls().begin())->GetFaces().begin();
    face->CheckPointering();
    EXPECT_EQ(6, FaceCount(shape));

    auto point = *face->GetStartEdge()->GetStartVertex();
    auto dir = (*face->GetStartEdge()->GetNext()->GetNext()->GetStartVertex() - point).Normalized();
    auto res = face->GetContourLineIntersections(dir,point);
    ASSERT_EQ(2, res.size()) << "two vertex test count failed";
    EXPECT_EQ(Face::ContourLineIntersection::Type::VertexIntersection, res[0].GetType()) << "two vertex test type 0 failed";
    EXPECT_EQ(Face::ContourLineIntersection::Type::VertexIntersection, res[1].GetType()) << "two vertex test type 1 failed";

    point = *face->GetStartEdge()->GetNext()->GetStartVertex();
    dir = (*face->GetStartEdge()->GetNext()->GetNext()->GetStartVertex() - *face->GetStartEdge()->GetStartVertex()).Normalized();
    res = face->GetContourLineIntersections(dir, point);
    ASSERT_EQ(1, res.size()) << "one vertex test count failed";
    EXPECT_EQ(Face::ContourLineIntersection::Type::VertexIntersection, res[0].GetType()) << "one vertex test type failed";

    point = *face->GetStartEdge()->GetStartVertex();
    dir = (*face->GetStartEdge()->GetNext()->GetStartVertex() - point).Normalized();
    point += *face->GetStartEdge()->GetNext()->GetNext()->GetStartVertex() * 0.5;
    res = face->GetContourLineIntersections(dir, point);
    ASSERT_EQ(2, res.size()) << "two edges test count failed";
    EXPECT_EQ(Face::ContourLineIntersection::Type::EdgeIntersection, res[0].GetType()) << "two edges test type 0 failed";
    EXPECT_EQ(Face::ContourLineIntersection::Type::EdgeIntersection, res[1].GetType()) << "two edges test type 1 failed";

    point = *face->GetStartEdge()->GetStartVertex();
    dir = (*face->GetStartEdge()->GetNext()->GetStartVertex() - point).Normalized();
    res = face->GetContourLineIntersections(dir, point);
    ASSERT_EQ(1, res.size()) << "on edge test count failed";
    EXPECT_EQ(Face::ContourLineIntersection::Type::EdgeIsIntersection, res[0].GetType()) << "on edges test type failed";

    point = *face->GetStartEdge()->GetStartVertex();
    dir = (*face->GetStartEdge()->GetNext()->GetStartVertex()*.5 + *face->GetStartEdge()->GetNext()->GetNext()->GetStartVertex()*.5 - point).Normalized();
    res = face->GetContourLineIntersections(dir, point);
    ASSERT_EQ(2, res.size()) << "one edge, one vertex test count failed";
    EXPECT_EQ(Face::ContourLineIntersection::Type::VertexIntersection, res[0].GetType()) << "one edge, one vertex test type 0 failed";
    EXPECT_EQ(Face::ContourLineIntersection::Type::EdgeIntersection, res[1].GetType()) << "one edge, one vertex test type 1 failed";

    point = *face->GetStartEdge()->GetStartVertex();
    dir = (*face->GetStartEdge()->GetNext()->GetNext()->GetStartVertex() - point).Normalized();
    point -= dir;
    dir = face->GetNormal()->Cross(dir);
    res = face->GetContourLineIntersections(dir, point);
    EXPECT_EQ(0, res.size()) << "complete miss test count failed";
}
