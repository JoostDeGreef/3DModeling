#include <algorithm>
#include <cassert>
#include <map>
#include <stack>
#include <unordered_map>
using namespace std;

#include "Aliases.h"
#include "RGBAColor.h"
#include "Face.h"
#include "Patch.h"
#include "Hull.h"
#include "Shape.h"
using namespace Geometry;

HullPtr Hull::Copy(Shape& newShape) const
{
    ForEachFace([](const FacePtr& face) { face->CheckPointering(); });

    HullPtr newHull = newShape.ConstructAndAddHull();
    newHull->SetOrientation(GetOrientation());
    newHull->SetBoundingShape(GetBoundingShape());
      
    std::unordered_map<Patch*, PatchPtr> patches;
    std::unordered_map<FacePtr, FacePtr> faces;
    std::unordered_map<EdgePtr, EdgePtr> edges;
    std::unordered_map<VertexPtr, VertexPtr> vertices;
    std::unordered_map<NormalPtr, NormalPtr> normals;
    std::unordered_map<ColorPtr, ColorPtr> colors;
    std::unordered_map<TextureCoordPtr, TextureCoordPtr> textureCoordinates;

    // create a new instance for each existing patch, face, edge, vertex, normal, color and texturecoordinate
    ForEachPatch([&](const PatchPtr& patch)
    {
        colors.emplace(patch->GetColor(), ColorPtr());
        patches.emplace(patch.get(), PatchPtr());
        patch->ForEachFace([&](const FacePtr& face)
        {
            normals.emplace(face->GetNormal(), NormalPtr());
            colors.emplace(face->GetColor(), ColorPtr());
            faces.emplace(face, FacePtr());
            face->ForEachEdge([&](const EdgePtr& edge)
            {
                edges.emplace(edge, EdgePtr());
                vertices.emplace(edge->GetStartVertex(), VertexPtr());
                normals.emplace(edge->GetStartNormal(), NormalPtr());
                colors.emplace(edge->GetStartColor(), ColorPtr());
                textureCoordinates.emplace(edge->GetStartTextureCoord(), TextureCoordPtr());
            });
        });
    });
    // duplicate shallow structures
    for (auto& vertexMap : vertices)
    {
        vertexMap.second = vertexMap.first == nullptr ? nullptr : make_shared<Vertex>(*vertexMap.first);
    }
    for (auto& normalMap : normals)
    {
        normalMap.second = normalMap.first == nullptr ? nullptr : make_shared<Normal>(*normalMap.first);
    }
    for (auto& colorMap : colors)
    {
        colorMap.second = colorMap.first == nullptr ? nullptr : make_shared<Color>(*colorMap.first);
    }
    for (auto& textureCoordinateMap : textureCoordinates)
    {
        textureCoordinateMap.second = textureCoordinateMap.first == nullptr ? nullptr : make_shared<TextureCoord>(*textureCoordinateMap.first);
    }
    // recreate complex structures
    for (auto& patchMap : patches)
    {
        patchMap.second = make_shared<Patch>(*newHull);
        newHull->AddPatch(patchMap.second);
        patchMap.second->SetBoundingShape(patchMap.first->GetBoundingShape());
        patchMap.second->SetColor(colors[patchMap.first->GetColor()]);
    }
    for (auto& faceMap : faces)
    {
        PatchPtr patch = patches[&faceMap.first->GetPatch()];
        assert(patch != nullptr);
        faceMap.second = make_shared<Face>(*patch);
        patch->AddFace(faceMap.second);
    }
    for (auto& edgeMap : edges)
    {
        VertexPtr& v = vertices[edgeMap.first->GetStartVertex()];
        assert(v != nullptr);
        FacePtr& f = faces[edgeMap.first->GetFace()];
        assert(f != nullptr);
        edgeMap.second = make_shared<Edge>(f,v);
        f->AddEdge(edgeMap.second);
    }
    for (auto& edgeMap : edges)
    {
        edgeMap.second->SetStartNormal(normals[edgeMap.first->GetStartNormal()]);
        edgeMap.second->SetTwin(edges[edgeMap.first->GetTwin()]);
        edgeMap.second->SetNext(edges[edgeMap.first->GetNext()]);
        edgeMap.second->SetPrev(edges[edgeMap.first->GetPrev()]);
        edgeMap.second->SetStartColor(colors[edgeMap.first->GetStartColor()]);
        edgeMap.second->SetStartTextureCoord(textureCoordinates[edgeMap.first->GetStartTextureCoord()]);
    }
    for (auto& faceMap : faces)
    {
        assert(faceMap.first->GetEdgeCount() == faceMap.second->GetEdgeCount());
        assert(faceMap.second->GetStartEdge() != nullptr);
        faceMap.second->SetNormal(normals[faceMap.first->GetNormal()]);
        faceMap.second->SetColor(colors[faceMap.first->GetColor()]);
    }
    newHull->ForEachFace([](const FacePtr& face) { face->CheckPointering(); });
    return newHull;
}

void Hull::SplitTrianglesIn4()
{
    size_t count = 0;
    std::vector<FacePtr> faces;
    std::unordered_set<EdgePtr> edges;
    ForEachFace([&count](const FacePtr& face) 
    {
        ++count;
        face->CheckPointering(); 
    });
    faces.reserve(count);
    ForEachFace([&edges,&faces](const FacePtr& face)
    {
        assert(3==face->GetEdgeCount());
        faces.emplace_back(face);
        face->ForEachEdge([&](const EdgePtr& edge)
        {
            if (edge < edge->GetTwin())
            {
                edges.insert(edge);
            }
        });
    });
    for (const EdgePtr& edge : edges)
    {
        edge->Split();
    }
    for (FacePtr& face : faces)
    {
        Patch& patch = face->GetPatch();
        // new faces
        FacePtr f0 = patch.ConstructAndAddFace();
        FacePtr f1 = patch.ConstructAndAddFace();
        FacePtr f2 = patch.ConstructAndAddFace();
        FacePtr f3 = patch.ConstructAndAddFace();
        // edges, with e01 pre-existing
        EdgePtr e01 = face->GetStartEdge();
        if (edges.find(e01) == edges.end())
        {
            e01 = e01->GetNext();
        }
        EdgePtr e12 = e01->GetNext();
        EdgePtr e23 = e12->GetNext();
        EdgePtr e34 = e23->GetNext();
        EdgePtr e45 = e34->GetNext();
        EdgePtr e50 = e45->GetNext();
        // newly created vertices
        VertexPtr v1 = e12->GetStartVertex();
        VertexPtr v3 = e34->GetStartVertex();
        VertexPtr v5 = e50->GetStartVertex();
        // normals at new vertices
        NormalPtr n1 = e12->GetStartNormal();
        NormalPtr n3 = e34->GetStartNormal();
        NormalPtr n5 = e50->GetStartNormal();
        // create new edges
        EdgePtr e13 = Face::ConstructAndAddEdge(f3, v1, n1);
        EdgePtr e31 = Face::ConstructAndAddEdge(f1, v3, n3);
        EdgePtr e35 = Face::ConstructAndAddEdge(f3, v3, n3);
        EdgePtr e53 = Face::ConstructAndAddEdge(f2, v5, n5);
        EdgePtr e15 = Face::ConstructAndAddEdge(f0, v1, n1);
        EdgePtr e51 = Face::ConstructAndAddEdge(f3, v5, n5);
        // set new edge twins
        e13->SetTwin(e31); e31->SetTwin(e13);
        e35->SetTwin(e53); e53->SetTwin(e35);
        e15->SetTwin(e51); e51->SetTwin(e15);
        // set next/prev
        auto Link = [](EdgePtr& e0, EdgePtr& e1) {e0->SetNext(e1); e1->SetPrev(e0); };
        Link(e01, e15); Link(e15, e50);
        Link(e23, e31); Link(e31, e12);
        Link(e45, e53); Link(e53, e34);
        Link(e13, e35); Link(e35, e51); Link(e51, e13);
        // connect existing edges to new face
        e01->SetFace(f0); e50->SetFace(f0);
        e12->SetFace(f1); e23->SetFace(f1);
        e34->SetFace(f2); e45->SetFace(f2);
        // add existing edges to faces
        f0->AddEdge(e01);
        f0->AddEdge(e50);
        f1->AddEdge(e12);
        f1->AddEdge(e23);
        f2->AddEdge(e34);
        f2->AddEdge(e45);
        // calculate face normals
        f0->CalcNormal();
        f1->CalcNormal();
        f2->CalcNormal();
        f3->CalcNormal();
        // check all faces
        f0->CheckPointering();
        f1->CheckPointering();
        f2->CheckPointering();
        f3->CheckPointering();
        // remove old face
        patch.RemoveFace(face);
    }
}

void Hull::Triangulate()
{
    std::vector<FacePtr> faces;
    ForEachFace([&faces](const FacePtr& face) 
    {
        faces.emplace_back(face);
        face->CheckPointering();
    });
    for (FacePtr& face : faces)
    {
        face->Triangulate();
    }
}
