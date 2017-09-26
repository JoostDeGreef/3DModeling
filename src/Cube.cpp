#include <vector>
using namespace std;

#include "Common.h"
using namespace Common;

#include "Geometry.h"
using namespace Geometry;

ShapePtr::Shared Cube::Construct()
{
    ShapePtr::Shared shape = Shape::Construct();

    // calculate all vertices
    VertexPtr::Shared A = GeoFactory::Construct<Vertex>(-1,-1, 1);
    VertexPtr::Shared B = GeoFactory::Construct<Vertex>( 1,-1, 1);
    VertexPtr::Shared C = GeoFactory::Construct<Vertex>( 1, 1, 1);
    VertexPtr::Shared D = GeoFactory::Construct<Vertex>(-1, 1, 1);
    VertexPtr::Shared E = GeoFactory::Construct<Vertex>(-1,-1,-1);
    VertexPtr::Shared F = GeoFactory::Construct<Vertex>(-1, 1,-1);
    VertexPtr::Shared G = GeoFactory::Construct<Vertex>( 1, 1,-1);
    VertexPtr::Shared H = GeoFactory::Construct<Vertex>( 1,-1,-1);
    vector<VertexPtr::Shared> vertices({ A,B,C,D,E,F,G,H });

    // normalize all vertices, just to be sure
    for (auto v : vertices)
    {
        v->Normalize();
    }

    // create the hull
    HullPtr::Raw hull = shape->ConstructAndAddHull();

    auto CreateFace = [&](VertexPtr::Shared& a, VertexPtr::Shared& b, VertexPtr::Shared& c, VertexPtr::Shared& d,
        EdgePtr::Raw& ab, EdgePtr::Raw& bc, EdgePtr::Raw& cd, EdgePtr::Raw& da)
    {
        FacePtr::Raw face = hull->ConstructAndAddPatch()->ConstructAndAddFace();
        ab = face->ConstructAndAddEdge(a);
        bc = face->ConstructAndAddEdge(b);
        cd = face->ConstructAndAddEdge(c);
        da = face->ConstructAndAddEdge(d);
        ab->SetNext(bc);  ab->SetPrev(da);
        bc->SetNext(cd);  bc->SetPrev(ab);
        cd->SetNext(da);  cd->SetPrev(bc);
        da->SetNext(ab);  da->SetPrev(cd);
        return face;
    };

    EdgePtr::Raw AB, BC, CD, DA, AE, EH, HB, BA, BH, HG, GC, CB;
    EdgePtr::Raw CG, GF, FD, DC, AD, DF, FE, EA, EF, FG, GH, HE;

    // create all the faces and extract their edge pointers
    FacePtr::Raw face1 = CreateFace(A, B, C, D, AB, BC, CD, DA);
    FacePtr::Raw face2 = CreateFace(A, E, H, B, AE, EH, HB, BA);
    FacePtr::Raw face3 = CreateFace(B, H, G, C, BH, HG, GC, CB);
    FacePtr::Raw face4 = CreateFace(C, G, F, D, CG, GF, FD, DC);
    FacePtr::Raw face5 = CreateFace(A, D, F, E, AD, DF, FE, EA);
    FacePtr::Raw face6 = CreateFace(E, F, G, H, EF, FG, GH, HE);

    auto Join = [](const EdgePtr::Raw& a, const EdgePtr::Raw& b)
    {
        a->SetTwin(b);
        b->SetTwin(a);
    };

    // join all twin edges
    Join(AB, BA);
    Join(BC, CB);
    Join(CD, DC);
    Join(AD, DA);

    Join(AE, EA);
    Join(BH, HB);
    Join(CG, GC);
    Join(DF, FD);

    Join(EH, HE);
    Join(GH, HG);
    Join(GF, FG);
    Join(EF, FE);
    
    // check geometric integrity
    shape->ForEachFace([](const FacePtr::Raw& face) {face->CheckPointering(); });

    return shape;
}

