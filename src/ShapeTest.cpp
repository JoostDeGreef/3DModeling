
#include "gtest/gtest.h"
using namespace testing;

#include "Shape.h"
#include "Cube.h"
#include "Dodecahedron.h"
using namespace Geometry;

class ShapeTest : public Test 
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

TEST_F(ShapeTest, Copy) 
{
    auto CopyShape = [](ShapePtr d)
    {
        std::set<VertexPtr> vertices;
        std::set<EdgePtr> edges;
        d->ForEachVertex([&vertices](const VertexPtr& vertex) { vertices.emplace(vertex); });
        d->ForEachEdge([&edges](const EdgePtr& edge) { edges.emplace(edge); });
        size_t vc0 = vertices.size();
        size_t ec0 = edges.size();
        ShapePtr s0 = std::make_shared<Shape>(*d);
        s0->ForEachVertex([&vertices](const VertexPtr& vertex) { vertices.emplace(vertex); });
        s0->ForEachEdge([&edges](const EdgePtr& edge) { edges.emplace(edge); });
        size_t vc1 = vertices.size();
        size_t ec1 = edges.size();
        ASSERT_EQ(vc0 * 2, vc1);
        ASSERT_EQ(ec0 * 2, ec1);
    };

    CopyShape(std::make_shared<Cube>());
    CopyShape(std::make_shared<Dodecahedron>());

}

TEST_F(ShapeTest, SplitTrianglesIn4)
{
    ShapePtr shape = std::make_shared<Cube>();
    shape->Triangulate();
    EXPECT_EQ(12, FaceCount(shape));
    shape->SplitTrianglesIn4();
    EXPECT_EQ(48, FaceCount(shape));
}

TEST_F(ShapeTest, SerializeBlob)
{
//    ShapePtr::Shared shape0 = Dodecahedron::Construct(12);
////    ShapePtr::Shared shape0 = Cube::Construct();
//    auto s = shape0->Serialize();
//    ShapePtr::Shared shape1 = Shape::Construct();
//    shape1->Deserialize(s);
//    int count0 = 0;
//    int count1 = 0;
//    shape0->ForEachVertex([&count0](const VertexPtr::Raw& vertex) { count0++; });
//    shape1->ForEachVertex([&count1](const VertexPtr::Raw& vertex) { count1++; });
//    ASSERT_EQ(count0,count1);
//    count0 = 0;
//    count1 = 0;
//    shape0->ForEachFace([&count0](const FacePtr::Raw& face) { count0++; });
//    shape1->ForEachFace([&count1](const FacePtr::Raw& face) { count1++; });
//    ASSERT_EQ(count0, count1);
}

TEST_F(ShapeTest, SerializeDB)
{
    ////ShapePtr::Shared shape0 = Dodecahedron::Construct(12);
    //ShapePtr::Shared shape0 = Cube::Construct();
    //Common::DB db;
    //db.open(":memory:");
    //shape0->Store(db);
    //ShapePtr::Shared shape1 = Shape::Construct();
    //shape1->Retrieve(db);
    //int count0 = 0;
    //int count1 = 0;
    //shape0->ForEachVertex([&count0](const VertexPtr::Raw& vertex) { count0++; });
    //shape1->ForEachVertex([&count1](const VertexPtr::Raw& vertex) { count1++; });
    //ASSERT_EQ(count0, count1);
    //count0 = 0;
    //count1 = 0;
    //shape0->ForEachFace([&count0](const FacePtr::Raw& face) { count0++; });
    //shape1->ForEachFace([&count1](const FacePtr::Raw& face) { count1++; });
    //ASSERT_EQ(count0, count1);
}
