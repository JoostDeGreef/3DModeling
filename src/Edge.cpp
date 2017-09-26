#include <vector>
using namespace std;

#include "Aliases.h"
#include "Edge.h"
#include "Face.h"
#include "Patch.h"
#include "Shape.h"
#include "RGBAColor.h"
using namespace Geometry;

void Edge::Split()
{
    VertexPtr vertex0 = GetStartVertex();
    VertexPtr vertex2 = GetEndVertex();
    VertexPtr vertex1 = make_shared<Vertex>(Middle(*vertex0, *vertex2));
    NormalPtr normal0 = GetStartNormal();
    NormalPtr normal2 = GetEndNormal();
    NormalPtr normal1;
    ColorPtr color0 = GetStartColor();
    ColorPtr color2 = GetEndColor();
    ColorPtr color1;

    FacePtr thisFace = GetFace();
    thisFace->CheckPointering();

    FacePtr twinFace = GetTwinFace();
    twinFace->CheckPointering();

    EdgePtr twin0 = GetTwin();
    EdgePtr twin1 = make_shared<Edge>(twinFace,vertex1);
    twinFace->AddEdge(twin1);

    EdgePtr this0 = twin0->GetTwin();
    EdgePtr this1 = make_shared<Edge>(thisFace,vertex1);
    thisFace->AddEdge(this1);

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
        normal1 = make_shared<Normal>(*normal0 + *normal2);
        normal1->Normalize();
        this1->SetStartNormal(normal1);
        twin1->SetStartNormal(normal1);
    }

    if (color0 && color2)
    {
        color1 = make_shared<Color>(color0->Mix(*color2));
        this1->SetStartColor(color1);
        twin1->SetStartColor(color1);
    }

    thisFace->CheckPointering();
    twinFace->CheckPointering();
}

