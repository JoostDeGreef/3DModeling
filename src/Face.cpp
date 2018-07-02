#include "Geometry.h"
using namespace std;
using namespace Geometry;

Face::~Face()
{}

void Face::CalcNormal()
{
    CheckPointering();
    Normal normal(0, 0, 0);
    VertexRaw p0 = (*m_edges.begin())->GetPrev()->GetStartVertex();
    ForEachVertex([&normal,&p0](const VertexRaw& p1)
    {
        normal += CrossProduct(*p0, *p1);
        p0 = p1;
    });
    double surface = normal.Length();
    m_normal = Construct<Normal>(normal / surface);
}

const std::vector<EdgeRaw> Face::GetEdgesOrdered() const
{
    std::vector<EdgeRaw> res;
    res.reserve(m_edges.size());
    ForEachEdge([&](const EdgeRaw& edge) 
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
    std::pair<std::pair<EdgeRaw,int>, std::pair<EdgeRaw,int>> FindExtremeVertices(bool AllowNeighbors)
    {
        std::vector<std::pair<VertexPtr, EdgeRaw>> edges;
        m_face->ForEachEdge([&](const EdgeRaw& edge)
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
        HullRaw hull = m_face->GetHull();
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
        EdgeRaw split0 = extremes.first.first;
        for (int i = 1; i < extremes.first.second / 2; ++i)
        {
            split0 = split0->GetNext();
        }
        EdgeRaw split1 = extremes.second.first;
        for (int i = 1; i < extremes.second.second / 2; ++i)
        {
            split1 = split1->GetNext();
        }
        // create new face and connecting edges
        FacePtr newFace0 = hull->ConstructAndAddFace();
        FacePtr newFace1 = hull->ConstructAndAddFace();

        EdgePtr edge0 = newFace0->ConstructAndAddEdge(split0->GetEndVertex());
        EdgePtr edge1 = newFace1->ConstructAndAddEdge(split1->GetEndVertex());

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
        auto MoveEdges = [&](const EdgeRaw& edge, const FacePtr& newFace)
        {
            EdgeRaw next = edge->GetNext();
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
        hull->RemoveFace(m_face);
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

void Face::ForEachEdge(std::function<void(const EdgeRaw& edge)> func) const
{
    EdgeRaw startEdge = GetStartEdge();
    EdgeRaw edge = startEdge;
    do
    {
        func(edge);
        edge = edge->GetNext();
    } while (edge != startEdge);
}

void Face::ForEachVertex(std::function<void(const VertexRaw& vertex)> func) const
{
    ForEachEdge([func](const EdgeRaw& edge)
    {
        func(edge->GetStartVertex());
    });
}

std::pair<FacePtr, FacePtr> Face::Split()
{
    FaceSplitter splitter(shared_from_this());
    return splitter.Split();
}

void Face::Triangulate()
{
    FaceSplitter splitter(shared_from_this());
    splitter.Triangulate();
}

#ifdef _DEBUG
void Face::CheckPointering() const
{
    // there should be edges
    assert(!m_edges.empty());
    // valid faces have more than 2 edges
    size_t count = 0;
    ForEachEdge([&count](const EdgeRaw& edge) {++count; });
    assert(count > 2);
    // all edges should be linked
    assert(m_edges.size() == count);
    // all linked edges should be 'owned' by this face
    ForEachEdge([&](const EdgeRaw& edge)
    {
        assert(m_edges.find(edge.lock()) != m_edges.end());
    });
    // check that twins know eachother
    ForEachEdge([](const EdgeRaw& edge)
    {
        assert(edge->GetTwin() != nullptr);
        assert(edge == edge->GetTwin()->GetTwin());
    });
    // check that every edge knows this face
    ForEachEdge([&](const EdgeRaw& edge)
    {
        assert(edge->GetFace() != nullptr);
    });
    // check that all edge normals are set or none
    int normalCount = 0;
    ForEachEdge([&normalCount](const EdgeRaw& edge)
    {
        if (edge->GetStartNormal())
        {
            normalCount++;
        }
    });
    assert(normalCount == 0 || normalCount == count);
}
#endif // _DEBUG

std::vector<Face::ContourLineIntersection> Face::GetContourLineIntersections(const Vertex& dir, const Vertex & point) const
{
    std::vector<std::tuple<EdgeRaw, Vector2d>> vertices;
    using namespace std::placeholders;
    // subtract point, rotate, drop element
    auto Apply = [&point](const VertexRaw& vertex)
    {
        // no rotation for now, the error seems small anyway.
        return *vertex - point;
    };
    // drop one element from the coordinate
    auto DropCoord = [&Apply,&vertices](const EdgeRaw & e, const unsigned int coord0, const unsigned int coord1)
    {
        auto p = Apply(e->GetStartVertex());
        vertices.emplace_back(e, Vector2d(p[coord0], p[coord1]));
    };
    // arrange the work
    unsigned int coord0;
    unsigned int coord1;
    auto norm = *GetNormal();
    norm.InnerProduct(norm); // absolute values for compare
    if (norm[0] >= norm[1])
    {
        if (norm[0] >= norm[2])
        {
            coord0 = 1;
            coord1 = 2;
        }
        else
        {
            coord0 = 0;
            coord1 = 1;
        }
    }
    else
    {
        coord0 = 0;
        coord1 = (norm[1] >= norm[2]) ? 2 : 1;
    }
    ForEachEdge([&DropCoord,&coord0,&coord1](const EdgeRaw & e) { DropCoord(e, coord0, coord1); });
    const auto dir2d = Vector2d(dir[coord0], dir[coord1]);
    // find intersections
    std::vector<Face::ContourLineIntersection> res;
    Line2d line0({ 0,0 }, dir2d);
    for(unsigned int i=0;i<vertices.size();++i)
    {
        unsigned int j = i + 1;
        if (j >= vertices.size())
        {
            j = 0;
        }
        Vector2d p0 = get<1>(vertices[i]);
        Vector2d p1 = get<1>(vertices[j]);
        Line2d line1(p0, p1);
        auto intersection = line0.CalculateIntersection(line1);
        switch (intersection.GetType(1))
        {
        case Line2d::Intersection::Start:
            res.emplace_back(get<0>(vertices[i]));
            break;
        case Line2d::Intersection::End:
            res.emplace_back(get<0>(vertices[j]));
            break;
        case Line2d::Intersection::On:
            res.emplace_back(get<0>(vertices[i]),intersection.s(1));
            break;
        default:
            break;
        }
    }
    // remove connected start and end intersections, duplicated intersections
    if (res.size() >= 2)
    {
        for (int i = (int)res.size() - 2; i >= 0; --i)
        {
            if (res[i] == res[i+1])
            {
                res.erase(res.begin() + i + 1);
            }
        }
    }
    if (res.size() >= 2 && res.front() == res.back())
    {
        res.erase(res.begin() + res.size() - 1);
    }
    // if there are 2 vertex intersections, and they belong to the same edge, join them.
    if (res.size() == 2
        && res.front().GetType() == ContourLineIntersection::Type::VertexIntersection
        && res.back().GetType() == ContourLineIntersection::Type::VertexIntersection)
    {
        auto SetEdgeIsIntersection = [&res](const EdgeRaw edge)
        {
            res.clear();
            res.emplace_back(ContourLineIntersection::Type::EdgeIsIntersection, edge, 0.0);            
        };
        if (res.front().GetEdge()->GetNext() == res.back().GetEdge())
        {
            SetEdgeIsIntersection(res.front().GetEdge());
        }
        else if (res.front().GetEdge() == res.back().GetEdge()->GetNext())
        {
            SetEdgeIsIntersection(res.back().GetEdge());
        }
    }
    return res;
}

Face::FaceIntersection Face::FindIntersection(FacePtr& other)
{
    FaceIntersection intersection(shared_from_this(), other);
    intersection.Calculate();
    return intersection;
}

void Face::FaceIntersection::Calculate()
{
    m_A->CheckPointering();
    m_B->CheckPointering();

    // get the face normals
    auto& normalA = m_A->GetNormal();
    auto& normalB = m_B->GetNormal();
    assert(normalA && normalB);

    // the direction of the line is orthogonal to both plane normals
    auto normalL = normalA->Cross(*normalB);

    // if the planes a parallel, return;
    if (normalL.LengthSquared() < 0.0001)
    {
        return;
    }

    // normalize the normal
    normalL.Normalize();

    // distance from the planes to the origin
    auto distanceA = -normalA->InnerProduct(*m_A->GetStartEdge()->GetStartVertex());
    auto distanceB = -normalB->InnerProduct(*m_B->GetStartEdge()->GetStartVertex());

    // find a point on the line
    auto pointL = (*normalA * distanceB - *normalB * distanceA).Cross(normalL) / normalL.InnerProduct(normalL);

    // intersect this line with the countour of A.
    auto intersectionsA = m_A->GetContourLineIntersections(normalL, pointL);
    switch(intersectionsA.size())
    {
    case 0: // no intersection
        return;
    case 1: // single point intersection
        // TODO
        m_points.emplace_back();
        return;
    default: // full line intersection
        break;
    }

// TODO: adjust normalL and pointL to make it the start and end on A

    // intersect this line with the countour of B.
    auto intersectionsB = m_B->GetContourLineIntersections(normalL, pointL);

// TODO: see if the intersection intervals overlap.
}


