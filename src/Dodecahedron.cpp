#include <vector>
#include <memory>
#include <cassert>
using namespace std;

#include "Dodecahedron.h"
#include "Quaternion.h"
#include "RotationMatrix.h"
using namespace Geometry;

Dodecahedron::Dodecahedron(int initialFaceCount)
    : Shape()
{
    const double t = (1 + sqrt(5.0))*0.5;
    const double ti = 2.0 / (1 + sqrt(5.0));
    const double s = 1 / sqrt(3.0);

    // calculate all vertices
    VertexPtr A = make_shared<Vertex>(s * t, s * 0, s * ti);
    VertexPtr B = make_shared<Vertex>(s *-t, s * 0, s * ti);
    VertexPtr C = make_shared<Vertex>(s *-t, s * 0, s *-ti);
    VertexPtr D = make_shared<Vertex>(s * t, s * 0, s *-ti);
    VertexPtr E = make_shared<Vertex>(s * ti, s * t, s * 0);
    VertexPtr F = make_shared<Vertex>(s * ti, s *-t, s * 0);
    VertexPtr G = make_shared<Vertex>(s *-ti, s *-t, s * 0);
    VertexPtr H = make_shared<Vertex>(s *-ti, s * t, s * 0);
    VertexPtr I = make_shared<Vertex>(s * 0, s * ti, s * t);
    VertexPtr J = make_shared<Vertex>(s * 0, s * ti, s *-t);
    VertexPtr K = make_shared<Vertex>(s * 0, s *-ti, s *-t);
    VertexPtr L = make_shared<Vertex>(s * 0, s *-ti, s * t);
    VertexPtr M = make_shared<Vertex>(s * 1, s * 1, s * 1);
    VertexPtr N = make_shared<Vertex>(s * 1, s *-1, s * 1);
    VertexPtr O = make_shared<Vertex>(s *-1, s *-1, s * 1);
    VertexPtr P = make_shared<Vertex>(s *-1, s * 1, s * 1);
    VertexPtr Q = make_shared<Vertex>(s *-1, s * 1, s *-1);
    VertexPtr R = make_shared<Vertex>(s * 1, s * 1, s *-1);
    VertexPtr S = make_shared<Vertex>(s * 1, s *-1, s *-1);
    VertexPtr T = make_shared<Vertex>(s *-1, s *-1, s *-1);
    vector<VertexPtr> vertices({ A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T });

    // normalize all vertices, just to be sure
    for (auto v : vertices)
    {
        v->Normalize();
    }

    // rotate vertices so that I ends up being 0,0,1
    Quat q(*I, Vertex(0, 1, 0));
    RotationMatrix3d rot(q);
    for (auto v : vertices)
    {
        *v = rot.Transform(*v);
    }

    // create the hull
    HullPtr hull = ConstructAndAddHull();

    auto CreateFace = [&](VertexPtr& a, VertexPtr& b, VertexPtr& c, VertexPtr& d, VertexPtr& e,
                          EdgePtr& ab, EdgePtr& bc, EdgePtr& cd, EdgePtr& de, EdgePtr& ea)
    {
        FacePtr face = hull->ConstructAndAddPatch()->ConstructAndAddFace();
        ab = Face::ConstructAndAddEdge(face, a);
        bc = Face::ConstructAndAddEdge(face, b);
        cd = Face::ConstructAndAddEdge(face, c);
        de = Face::ConstructAndAddEdge(face, d);
        ea = Face::ConstructAndAddEdge(face, e);
        ab->SetNext(bc);  ab->SetPrev(ea);
        bc->SetNext(cd);  bc->SetPrev(ab);
        cd->SetNext(de);  cd->SetPrev(bc);
        de->SetNext(ea);  de->SetPrev(cd);
        ea->SetNext(ab);  ea->SetPrev(de);
        return face;
    };

    EdgePtr AD, DR, RE, EM, MA, EH, HP, PI, IM, ME;
    EdgePtr ER, RJ, JQ, QH, HE, BP, PH, HQ, QC, CB;
    EdgePtr CQ, QJ, JK, KT, TC, BC, CT, TG, GO, OB;
    EdgePtr FG, GT, TK, KS, SF, FN, NL, LO, OG, GF;
    EdgePtr AN, NF, FS, SD, DA, AM, MI, IL, LN, NA;
    EdgePtr BO, OL, LI, IP, PB, DS, SK, KJ, JR, RD;

    // create all the faces and extract their edge pointers
    FacePtr face1 = CreateFace(A, D, R, E, M, AD, DR, RE, EM, MA);
    FacePtr face2 = CreateFace(E, H, P, I, M, EH, HP, PI, IM, ME);
    FacePtr face3 = CreateFace(E, R, J, Q, H, ER, RJ, JQ, QH, HE);
    FacePtr face4 = CreateFace(B, P, H, Q, C, BP, PH, HQ, QC, CB);
    FacePtr face5 = CreateFace(C, Q, J, K, T, CQ, QJ, JK, KT, TC);
    FacePtr face6 = CreateFace(B, C, T, G, O, BC, CT, TG, GO, OB);
    FacePtr face7 = CreateFace(F, G, T, K, S, FG, GT, TK, KS, SF);
    FacePtr face8 = CreateFace(F, N, L, O, G, FN, NL, LO, OG, GF);
    FacePtr face9 = CreateFace(A, N, F, S, D, AN, NF, FS, SD, DA);
    FacePtr face10 = CreateFace(A, M, I, L, N, AM, MI, IL, LN, NA);
    FacePtr face11 = CreateFace(B, O, L, I, P, BO, OL, LI, IP, PB);
    FacePtr face12 = CreateFace(D, S, K, J, R, DS, SK, KJ, JR, RD);

    auto Join = [](const EdgePtr& a, const EdgePtr& b)
    {
        a->SetTwin(b);
        b->SetTwin(a);
    };

    // join all twin edges
    Join(ME, EM);
    Join(ER, RE);
    Join(HE, EH);
    Join(PH, HP);
    Join(HQ, QH);
    Join(CQ, QC);
    Join(QJ, JQ);
    Join(BC, CB);
    Join(CT, TC);
    Join(GT, TG);
    Join(TK, KT);
    Join(OG, GO);
    Join(GF, FG);
    Join(NF, FN);
    Join(FS, SF);
    Join(DA, AD);
    Join(AM, MA);
    Join(MI, IM);
    Join(LN, NL);
    Join(NA, AN);
    Join(BO, OB);
    Join(OL, LO);
    Join(LI, IL);
    Join(IP, PI);
    Join(PB, BP);
    Join(DS, SD);
    Join(SK, KS);
    Join(KJ, JK);
    Join(JR, RJ);
    Join(RD, DR);

    // check geometric integrity
    ForEachFace([](const FacePtr& face) {face->CheckPointering(); });

    Refine(initialFaceCount);
}

void Dodecahedron::InitialRefinement()
{
    std::vector<FacePtr> faces;
    faces.reserve(12);
    ForEachFace([&faces](const FacePtr& face){faces.push_back(face);});
    assert(12 == faces.size());
    for (int i = 0; i < 12; ++i)
    {
        FacePtr& face = faces[i];
        Patch& patch = face->GetPatch();
        Vertex center(0, 0, 0);
        vector<VertexPtr> vertices;
        vector<EdgePtr> edges;
        face->ForEachEdge([&face,&center,&vertices,&edges](const EdgePtr& edge)
        {
            VertexPtr vertex = edge->GetStartVertex();
            center += *vertex; 
            vertices.push_back(vertex);
            edges.push_back(edge);
        });
        center.Normalize();
        vertices.push_back(vertices.front());
        edges.push_back(edges.front());

        VertexPtr centerPtr = std::make_shared<Vertex>(center);
        std::vector<FacePtr> newFaces({ patch.ConstructAndAddFace(), patch.ConstructAndAddFace(), patch.ConstructAndAddFace(), patch.ConstructAndAddFace(), patch.ConstructAndAddFace() });
        for (size_t i = 0; i < 5; ++i)
        {
            EdgePtr edge0 = edges[i]; edge0->SetFace(newFaces[i]);
            EdgePtr edge1 = Face::ConstructAndAddEdge(newFaces[i],vertices[i + 1]);
            EdgePtr edge2 = Face::ConstructAndAddEdge(newFaces[i],centerPtr);
            newFaces[i]->AddEdge(edge0);
            edge0->SetNext(edge1); edge0->SetPrev(edge2);
            edge1->SetNext(edge2); edge1->SetPrev(edge0);
            edge2->SetNext(edge0); edge2->SetPrev(edge1);
        }
        for (size_t i = 0; i < 5; ++i)
        {
            EdgePtr edge0 = edges[i + 0]->GetNext();
            EdgePtr edge1 = edges[i + 1]->GetPrev();
            edge0->SetTwin(edge1);
            edge1->SetTwin(edge0);
        }
        patch.RemoveFace(face);
    }
    ForEachFace([](const FacePtr& face) {face->CalcNormal(); });
    ForEachFace([](const FacePtr& face) {face->CheckPointering(); });
}

void Dodecahedron::Refine(int initialFaceCount)
{
    int faceCount = 0;
    ForEachFace([&faceCount](const FacePtr& face) {faceCount++; });

    if (initialFaceCount > faceCount)
    {
        // refine with knowledge of existing shapes, creates triangles
        InitialRefinement();

        faceCount = 0;
        ForEachFace([&faceCount](const FacePtr& face) {faceCount++; });

// TODO
        // the number of patches should be larger; turn every face into a patch (60)
        //std::vector<PatchPtr> patches;
        //ForEachPatch([&patches](const PatchPtr& patch)
        //{
        //    patches.emplace_back(patch);
        //});
        //for (auto& patch : patches)
        //{
        //    auto& hull = patch->GetHull();
        //    auto& faces = patch->GetFaces();
        //    while (faces.size() > 1)
        //    {
        //        const PatchPtr& p = hull->ConstructAndAddPatch(patch->GetColor());
        //        faces.back()->SetPatch(p);
        //        p->GetFaces().emplace_back(faces.back());
        //        faces.pop_back();
        //    }
        //}

        // give all patches a boundingsphere
        ForEachPatch([](const PatchPtr& patch)
        {
            patch->CalculateBoundingShape();
        });

        // divide all triangles in 4
        while (faceCount < initialFaceCount)
        {
            SplitTrianglesIn4();
            faceCount *= 4;
        }
        // triangles only, required in order to reallign vertices to sphere
        Triangulate();
    }
    // move all vertices to the sphere (dist 1.0 from origin) and correct the face normals
    ForEachVertex([](const VertexPtr& vertex) {vertex->Normalize(); });
    ForEachFace([](const FacePtr& face) {face->CalcNormal(); });
    // create perfect sphere edge normals
    ForEachEdge([&](const EdgePtr& edge)
    {
        // give all edges at this vertex the same normal
        if (!edge->GetStartNormal())
        {
            NormalPtr normal = std::make_shared<Normal>(*edge->GetStartVertex());
            edge->ForEachEdgeAtStartVertex([normal](const EdgePtr& edge) {edge->SetStartNormal(normal); });
        }
    });
}



