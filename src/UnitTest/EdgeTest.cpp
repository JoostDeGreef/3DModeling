#include "CommonTestFunctionality.h"

class EdgeTest : public Test 
{
protected:
	virtual void SetUp() 
    {
    }

    virtual void TearDown() 
    {
    }
};

TEST_F(EdgeTest, Split) 
{
    Shape shape;
    HullPtr hull = shape.ConstructAndAddHull();
    PatchPtr patch = hull->ConstructAndAddPatch();
    FacePtr f_top = patch->ConstructAndAddFace();
    FacePtr f_bottom = patch->ConstructAndAddFace();
    VertexPtr v0 = Construct<Vertex>(0, 0, 0);
    VertexPtr v1 = Construct<Vertex>(1, 0, 0);
    VertexPtr v2 = Construct<Vertex>(0, 1, 0);
    EdgePtr e0t = f_top->ConstructAndAddEdge(v0);
    EdgePtr e1t = f_top->ConstructAndAddEdge(v1);
    EdgePtr e2t = f_top->ConstructAndAddEdge(v2);
    EdgePtr e0b = f_bottom->ConstructAndAddEdge(v1);
    EdgePtr e1b = f_bottom->ConstructAndAddEdge(v0);
    EdgePtr e2b = f_bottom->ConstructAndAddEdge(v2);
    e0t->SetTwin(e0b); e0b->SetTwin(e0t);
    e1t->SetTwin(e2b); e2b->SetTwin(e1t);
    e2t->SetTwin(e1b); e1b->SetTwin(e2t);
    e0t->SetNext(e1t); e1t->SetNext(e2t); e2t->SetNext(e0t);
    e0b->SetNext(e1b); e1b->SetNext(e2b); e2b->SetNext(e0b);
    e0t->SetPrev(e2t); e2t->SetPrev(e1t); e1t->SetPrev(e0t);
    e0b->SetPrev(e2b); e2b->SetPrev(e1b); e1b->SetPrev(e0b);
    f_top->CalcNormal();
    f_bottom->CalcNormal();
    EXPECT_EQ(3, f_bottom->GetEdgeCount());
    f_top->CheckPointering();
    f_bottom->CheckPointering();
    e0t->Split();
    EXPECT_EQ(4, f_bottom->GetEdgeCount());
    f_top->CheckPointering();
    f_bottom->CheckPointering();
}
