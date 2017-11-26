#include "CommonTestFunctionality.h"
#include "SQLiteDB.h"

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
        shape->ForEachFace([&count](const FaceRaw& face)
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
        std::set<VertexRaw> vertices;
        std::set<EdgeRaw> edges;
        d->ForEachVertex([&vertices](const VertexRaw& vertex) { vertices.emplace(vertex); });
        d->ForEachEdge([&edges](const EdgeRaw& edge) { edges.emplace(edge); });
        size_t vc0 = vertices.size();
        size_t ec0 = edges.size();
        ShapePtr s0 = Construct<Shape>(*d);
        s0->ForEachVertex([&vertices](const VertexRaw& vertex) { vertices.emplace(vertex); });
        s0->ForEachEdge([&edges](const EdgeRaw& edge) { edges.emplace(edge); });
        size_t vc1 = vertices.size();
        size_t ec1 = edges.size();
        ASSERT_EQ(vc0 * 2, vc1);
        ASSERT_EQ(ec0 * 2, ec1);
    };

    CopyShape(Construct<Cube>());
    CopyShape(Construct<Dodecahedron>());

}

TEST_F(ShapeTest, SplitTrianglesIn4)
{
    ShapePtr shape = Construct<Cube>();
    shape->Triangulate();
    EXPECT_EQ(12, FaceCount(shape));
    shape->SplitTrianglesIn4();
    EXPECT_EQ(48, FaceCount(shape));
}

TEST_F(ShapeTest, StoreRetrieve)
{
    ShapePtr shape0 = Construct<Cube>();
    shape0->SetBoundingShape(BoundingShape3d(Vertex(1, 2, 3), Vertex(4, 5, 6)));
    SQLite::DB db;
    db.Open(":memory:",false);
    shape0->Store(db);
    ShapePtr shape1 = Construct<Shape>();
    shape1->Retrieve(db);
    int count0 = 0;
    int count1 = 0;
    shape0->ForEachVertex([&count0](const VertexRaw& vertex) { count0++; });
    shape1->ForEachVertex([&count1](const VertexRaw& vertex) { count1++; });
    ASSERT_EQ(count0, count1);
    count0 = 0;
    count1 = 0;
    shape0->ForEachFace([&count0](const FaceRaw& face) { count0++; });
    shape1->ForEachFace([&count1](const FaceRaw& face) { count1++; });
    ASSERT_EQ(count0, count1);
    ASSERT_EQ(shape0->GetBoundingShape(),shape1->GetBoundingShape());
}
