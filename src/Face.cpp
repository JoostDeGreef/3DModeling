#include <cassert>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "Aliases.h"
#include "Face.h"
#include "Patch.h"
#include "Shape.h"
#include "Vector.h"
using namespace Geometry;

Face::~Face()
{}

void Face::CalcNormal()
{
    CheckPointering();
    Normal normal(0, 0, 0);
    VertexPtr p0 = (*m_edges.begin())->GetPrev()->GetStartVertex();
    ForEachVertex([&normal,&p0](const VertexPtr& p1)
    {
        normal += CrossProduct(*p0, *p1);
        p0 = p1;
    });
    double surface = normal.Length();
    m_normal = Normal::Construct(normal / surface);
}

const std::vector<EdgePtr> Face::GetEdgesOrdered() const
{
    std::vector<EdgePtr> res;
    res.reserve(m_edges.size());
    ForEachEdge([&](EdgePtr edge) 
    {
        res.emplace_back(edge);
    });
    return res;
}


class FaceSplitter
{
    FacePtr m_face;
public:
    FaceSplitter(const FacePtr& face)
        : m_face(face)
    {
        m_face->CheckPointering();
    }

private:
    // return two strips which connect the 'extreme' points
    std::pair<std::pair<EdgePtr,int>, std::pair<EdgePtr,int>> FindExtremeVertices(bool AllowNeighbors)
    {
        std::vector<std::pair<VertexPtr, EdgePtr>> edges;
        m_face->ForEachEdge([&](const EdgePtr& edge)
        {
            edges.emplace_back(edge->GetStartVertex(),edge);
        });
        double dist = -1;
        int index0 = -1;
        int index1 = -1;
        // find the 2 vertices furthest apart
        size_t offset = AllowNeighbors ? 0 : 1;
        for (size_t i = 0; i<edges.size() - offset; ++i)
        {
            for (size_t j = i + 1 + offset; j<edges.size(); ++j)
            {
                double l = DistanceSquared(*edges[j].first, *edges[i].first);
                if (l > dist)
                {
                    index0 = (int)i;
                    index1 = (int)j;
                    dist = l;
                }
            }
        }
        assert(index0 != -1 && index1 != -1);
        return std::make_pair(
            std::make_pair(edges[index0].second, index1 - index0), 
            std::make_pair(edges[index1].second, (int)edges.size() - (index1 - index0)));
    }

    std::pair<FacePtr, FacePtr> SplitCore(bool AddVertices = true)
    {
        // check the input face is valid
        assert(m_face->GetEdgeCount() >= (size_t)(AddVertices?3:4));
        // parent objects
        Patch& patch = m_face->GetPatch();
        // find the two extreme vertices
        auto extremes = FindExtremeVertices(AddVertices);
        // split edges if needed
        if (extremes.first.second < 2)
        {
            extremes.first.first->Split();
            extremes.first.second++;
        }
        if (extremes.second.second < 2)
        {
            extremes.second.first->Split();
            extremes.second.second++;
        }
        // get the 2 edges _after_ which the connecting edges should be attached
        EdgePtr split0 = extremes.first.first;
        for (int i = 1; i < extremes.first.second / 2; ++i)
        {
            split0 = split0->GetNext();
        }
        EdgePtr split1 = extremes.second.first;
        for (int i = 1; i < extremes.second.second / 2; ++i)
        {
            split1 = split1->GetNext();
        }
        // create new face and connecting edges
        FacePtr newFace0 = Face::Construct(patch);
        FacePtr newFace1 = Face::Construct(patch);
        patch.AddFace(newFace0);
        patch.AddFace(newFace1);

        EdgePtr edge0 = Face::ConstructAndAddEdge(newFace0,split0->GetEndVertex());
        EdgePtr edge1 = Face::ConstructAndAddEdge(newFace1,split1->GetEndVertex());

        edge0->SetStartNormal(split0->GetEndNormal());
        edge1->SetStartNormal(split1->GetEndNormal());

        edge0->SetTwin(edge1);
        edge1->SetTwin(edge0);
        edge0->SetPrev(split0);
        edge0->SetNext(split1->GetNext());
        edge1->SetPrev(split1);
        edge1->SetNext(split0->GetNext());

        split0->GetNext()->SetPrev(edge1);
        split0->SetNext(edge0);
        split1->GetNext()->SetPrev(edge0);
        split1->SetNext(edge1);
        
        // move all the edges to their new face
        auto MoveEdges = [&](const EdgePtr& edge, const FacePtr& newFace)
        {
            EdgePtr next = edge->GetNext();
            while (next != edge)
            {
                newFace->AddEdge(next);
                next->SetFace(newFace);
                next = next->GetNext();
            }
        };
        MoveEdges(edge0, newFace0);
        MoveEdges(edge1, newFace1);

        // face color
        newFace0->SetColor(m_face->GetColor());
        newFace1->SetColor(m_face->GetColor());
        // remove the original face
        patch.RemoveFace(m_face);
        // update and check
        newFace0->CalcNormal();
        newFace1->CalcNormal();
        newFace0->CheckPointering();
        newFace1->CheckPointering();
        // return both faces 
        return std::make_pair(newFace0, newFace1);
    }

public:
    std::pair<FacePtr, FacePtr> Split()
    {
        return SplitCore();
    }
    
    void Triangulate()
    {
        if (m_face->GetEdgeCount() > 3)
        {
            auto faces = SplitCore(false);
            m_face = faces.first;
            Triangulate();
            m_face = faces.second;
            Triangulate();
        }
    }

};

std::pair<FacePtr, FacePtr> Face::Split()
{
    FaceSplitter splitter(this->GetStartEdge()->GetFace());
    return splitter.Split();
}

void Face::Triangulate()
{
    FaceSplitter splitter(this->GetStartEdge()->GetFace());
    splitter.Triangulate();
}

#ifdef _DEBUG
void Face::CheckPointering() const
{
    // there should be edges
    assert(!m_edges.empty());
    // valid faces have more than 2 edges
    size_t count = 0;
    ForEachEdge([&count](const EdgePtr& edge) {++count; });
    assert(count > 2);
    // all edges should be linked
    assert(m_edges.size() == count);
    // all linked edges should be 'owned' by this face
    ForEachEdge([&](const EdgePtr& edge)
    {
        assert(m_edges.find(edge) != m_edges.end());
    });
    // check that twins know eachother
    ForEachEdge([](const EdgePtr& edge)
    {
        assert(edge->GetTwin() != nullptr);
        assert(edge == edge->GetTwin()->GetTwin());
    });
    // check that every edge knows this face
    ForEachEdge([&](const EdgePtr& edge)
    {
        assert(edge->GetFace() != nullptr);
    });
    // check that all edge normals are set or none
    int normalCount = 0;
    ForEachEdge([&normalCount](const EdgePtr& edge)
    {
        if (edge->GetStartNormal())
        {
            normalCount++;
        }
    });
    assert(normalCount == 0 || normalCount == count);
}
#endif // _DEBUG
