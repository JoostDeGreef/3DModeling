#include "Geometry.h"
using namespace std;
using namespace Geometry;

Cube::Cube()
{
    // calculate all vertices
    VertexPtr A = Construct<Vertex>(-1,-1, 1);
    VertexPtr B = Construct<Vertex>( 1,-1, 1);
    VertexPtr C = Construct<Vertex>( 1, 1, 1);
    VertexPtr D = Construct<Vertex>(-1, 1, 1);
    VertexPtr E = Construct<Vertex>(-1,-1,-1);
    VertexPtr F = Construct<Vertex>(-1, 1,-1);
    VertexPtr G = Construct<Vertex>( 1, 1,-1);
    VertexPtr H = Construct<Vertex>( 1,-1,-1);
    vector<VertexPtr> vertices({ A,B,C,D,E,F,G,H });

    // create the hull
    HullPtr hull = ConstructAndAddHull();

    auto CreateFace = [&](VertexPtr& a, VertexPtr& b, VertexPtr& c, VertexPtr& d,
                          EdgePtr& ab, EdgePtr& bc, EdgePtr& cd, EdgePtr& da)
    {
        FacePtr face = hull->ConstructAndAddPatch()->ConstructAndAddFace();
        ab = Face::ConstructAndAddEdge(face,a);
        bc = Face::ConstructAndAddEdge(face,b);
        cd = Face::ConstructAndAddEdge(face,c);
        da = Face::ConstructAndAddEdge(face,d);
        ab->SetNext(bc);  ab->SetPrev(da);
        bc->SetNext(cd);  bc->SetPrev(ab);
        cd->SetNext(da);  cd->SetPrev(bc);
        da->SetNext(ab);  da->SetPrev(cd);
        return face;
    };

    EdgePtr AB, BC, CD, DA, AE, EH, HB, BA, BH, HG, GC, CB;
    EdgePtr CG, GF, FD, DC, AD, DF, FE, EA, EF, FG, GH, HE;

    // create all the faces and extract their edge pointers
    FacePtr face1 = CreateFace(A, B, C, D, AB, BC, CD, DA);
    FacePtr face2 = CreateFace(A, E, H, B, AE, EH, HB, BA);
    FacePtr face3 = CreateFace(B, H, G, C, BH, HG, GC, CB);
    FacePtr face4 = CreateFace(C, G, F, D, CG, GF, FD, DC);
    FacePtr face5 = CreateFace(A, D, F, E, AD, DF, FE, EA);
    FacePtr face6 = CreateFace(E, F, G, H, EF, FG, GH, HE);

    auto Join = [](const EdgePtr& a, const EdgePtr& b)
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
    ForEachFace([](const FacePtr& face) {face->CheckPointering(); });
}

