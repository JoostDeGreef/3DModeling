#include "Geometry.h"
using namespace std;
using namespace Geometry;

void Edge::Split()
{
    VertexPtr vertex0 = GetStartVertex();
    VertexPtr vertex2 = GetEndVertex();
    VertexPtr vertex1 = Construct<Vertex>(Middle(*vertex0, *vertex2));
    NormalPtr normal0 = GetStartNormal();
    NormalPtr normal2 = GetEndNormal();
    NormalPtr normal1;
    ColorPtr color0 = GetStartColor();
    ColorPtr color2 = GetEndColor();
    ColorPtr color1;

    FaceRaw thisFace = GetFace();
    thisFace->CheckPointering();

    FaceRaw twinFace = GetTwinFace();
    twinFace->CheckPointering();

    EdgeRaw twin0 = GetTwin();
    EdgeRaw twin1 = twinFace->ConstructAndAddEdge(vertex1);

    EdgeRaw this0 = twin0->GetTwin();
    EdgeRaw this1 = thisFace->ConstructAndAddEdge(vertex1);

    this0->SetTwin(twin1); 
    this1->SetTwin(twin0);
    twin0->SetTwin(this1);
    twin1->SetTwin(this0);

    this1->SetNext(this0->GetNext());
    this1->SetPrev(this0);
    this0->SetNext(this1);
    this1->GetNext()->SetPrev(this1);

    twin1->SetNext(twin0->GetNext());
    twin1->SetPrev(twin0);
    twin0->SetNext(twin1);
    twin1->GetNext()->SetPrev(twin1);

    if (normal0 && normal2)
    {
        normal1 = Construct<Normal>(*normal0 + *normal2);
        normal1->Normalize();
        this1->SetStartNormal(normal1);
        twin1->SetStartNormal(normal1);
    }

    if (color0 && color2)
    {
        color1 = Construct<Color>(color0->Mix(*color2));
        this1->SetStartColor(color1);
        twin1->SetStartColor(color1);
    }

    thisFace->CheckPointering();
    twinFace->CheckPointering();
}

void Edge::ForEachEdgeAtStartVertex(std::function<void(const EdgeRaw& edge)> func) const
{
    EdgeRaw me = const_cast<Edge*>(this);
    EdgeRaw next = me;
    do
    {
        func(next);
        next = next->GetTwin()->GetNext();
    } while (next != me);
}

