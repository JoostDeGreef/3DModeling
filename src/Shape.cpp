#include <future>
#include <chrono>
using namespace std;

#include "Geometry.h"
using namespace Geometry;

namespace
{
    size_t SerializationVersion = 1;
}

Shape::Shape()
    : m_hulls()
    , m_boundingShape()
{    
}

Shape::Shape(const this_type &other)
    : m_hulls()
    , m_boundingShape(other.m_boundingShape)
{
    for (const auto& hull : other.m_hulls)
    {
        hull->Copy(*this);
    }
}

Shape::Shape(this_type &&other)
    : m_hulls()
    , m_boundingShape()
{
    other.m_hulls.swap(m_hulls);
    std::swap(m_boundingShape, other.m_boundingShape);
}

Shape::~Shape()
{
    Clear();
}

void Shape::ForEachHull(std::function<void(const HullRaw& hull)> func) const
{
    for (const HullPtr& hull : GetHulls())
    {
        func(hull);
    }
}

void Shape::ForEachFace(std::function<void(const FaceRaw& facePtr)> func) const
{
    ForEachHull([func](const HullRaw& hull) 
    { 
        hull->ForEachFace(func); 
    });
}

void Shape::ForEachEdge(std::function<void(const EdgeRaw& edgePtr)> func) const
{
    ForEachHull([func](const HullRaw& hull) 
    { 
        hull->ForEachEdge(func); 
    });
}

void Shape::ForEachVertex(std::function<void(const VertexRaw& vertexPtr)> func) const
{
    ForEachHull([func](const HullRaw& hull)
    {
        hull->ForEachVertex(func);
    });
}

void Shape::ParallelForEachHull(std::function<void(const HullRaw& hull)> func) const
{
    // run the command on the first hull deferred, on all following hulls async.
    std::vector<std::future<void>> futures;
    for (const HullPtr& hull : GetHulls())
    {
        futures.emplace_back(std::async(futures.empty() ? (std::launch::deferred) : (std::launch::async), func, hull));
    }
    for (const auto& future : futures)
    {
        future.wait();
    }
}

void Shape::ParallelForEachFace(std::function<void(const FaceRaw& facePtr)> func) const
{
    ParallelForEachHull([func](const HullRaw& hull)
    {
        hull->ForEachFace(func);
    });
}

void Shape::ParallelForEachEdge(std::function<void(const EdgeRaw& edgePtr)> func) const
{
    ParallelForEachHull([func](const HullRaw& hull)
    {
        hull->ForEachEdge(func);
    });
}

void Shape::ParallelForEachVertex(std::function<void(const VertexRaw& vertexPtr)> func) const
{
    ParallelForEachHull([func](const HullRaw& hull)
    {
        hull->ForEachVertex(func);
    });
}

void Shape::SplitTrianglesIn4()
{
    ParallelForEachHull([](const HullRaw& hull)
    {
        hull->SplitTrianglesIn4(); 
    });
}

void Shape::Triangulate()
{
    ParallelForEachHull([](const HullRaw& hull)
    {
        hull->Triangulate(); 
    });
}

void Shape::Scale(const double factor)
{
    ParallelForEachHull([factor](const HullRaw& hull)
    {
        hull->Scale(factor);
    });
}

void Shape::Translate(const Vector3d& translation)
{
    ParallelForEachHull([translation](const HullRaw& hull)
    {
        hull->Translate(translation);
    });
}

void Shape::Clear()
{
    m_hulls.clear();
    m_boundingShape.Clear();
}

// Helper for Store
template<typename T>
static void StoreVectorMap(SQLite::DB& db,const std::unordered_map<T,size_t>& m,const std::string& table)
{
    SQLite::Statement stat;
    db.ExecDML("DROP TABLE IF EXISTS %1%", table);
    switch (T::element_type::dimension)
    {
    case 2:
        db.ExecDML("CREATE TABLE %1%(Id INTEGER PRIMARY KEY,x FLOAT,y FLOAT) WITHOUT ROWID", table);
        stat = db.CompileStatement("INSERT INTO %1%(Id,x,y) VALUES(?1,?2,?3)", table);
        break;
    default:
        assert(false); // Unsupported dimension
    case 3:
        db.ExecDML("CREATE TABLE %1%(Id INTEGER PRIMARY KEY,x FLOAT,y FLOAT,z FLOAT) WITHOUT ROWID", table);
        stat = db.CompileStatement("INSERT INTO %1%(Id,x,y,z) VALUES(?1,?2,?3,?4)", table);
        break;
    }
    for (auto& item : m)
    {
        if (item.first) // skip null values
        {
            stat.Reset();
            stat.Bind(1, (int64_t)item.second);
            for (int i = 0; i < T::element_type::dimension; ++i)
            {
                stat.Bind(2+i, (*item.first)[i]);
            }
            stat.ExecDML();
        }
    }
}

// Helper for Retrieve
template<typename T>
static void ReadVectorMap(SQLite::DB& db, std::unordered_map<size_t, std::shared_ptr<T>>& m, const std::string& table)
{
    SQLite::Query query;
    switch (T::dimension)
    {
    case 2:
        query = db.ExecQuery("SELECT Id,x,y FROM %1%",table);
        break;
    default:
        assert(false); // Unsupported dimension
    case 3:
        query = db.ExecQuery("SELECT Id,x,y,z FROM %1%",table);
        break;
    }

    for (;!query.IsEOF(); query.NextRow())
    {
        int64_t Id = query.GetInt64Field(0);
        T t;
        for (int i = 0; i < T::dimension; ++i)
        {
            t[i] = query.GetFloatField(1+i);
        }
        m.emplace(Id, Construct<T>(t));
    }
    m.emplace(0, nullptr);
}

namespace 
{
    struct Orientation
    {
        static int64_t Convert(Hull::Orientation orientation)
        {
            switch (orientation)
            {
            case Hull::Orientation::Inward: return 0;
            case Hull::Orientation::Outward: return 1;
            default:
                assert(false); // wickedly wrong orientation value!
                return -1;
            }
        }
        static Hull::Orientation Convert(int64_t orientation)
        {
            switch (orientation)
            {
            case 0: return Hull::Orientation::Inward;
            default:
                assert(false); // wickedly wrong orientation value!
            case 1: return Hull::Orientation::Outward;
            }
        }
    };
    struct BoundingShapeType
    {
        static int64_t Convert(BoundingShape3d::Type type)
        {
            switch (type)
            {
            default:
                assert(false); // wickedly wrong type value!
            case BoundingShape3d::Type::Unknown: return 0;
            case BoundingShape3d::Type::Ball: return 1;
            case BoundingShape3d::Type::Box: return 2;
            }
        }
        static BoundingShape3d::Type Convert(int64_t type)
        {
            switch (type)
            {
            default:
                assert(false); // wickedly wrong orientation value!
            case 0: return BoundingShape3d::Type::Unknown;
            case 1: return BoundingShape3d::Type::Ball;
            case 2: return BoundingShape3d::Type::Box;
            }
        }
    };
}

void Shape::Store(SQLite::DB& db) const
{
    ForEachFace([](const FaceRaw& face) {face->CheckPointering(); });

    std::unordered_map<HullRaw, size_t> hulls;
    std::unordered_map<EdgeRaw, size_t> edges;
    std::unordered_map<VertexRaw, size_t> vertices;
    std::unordered_map<NormalRaw, size_t> normals;
    std::unordered_map<TextureCoordRaw, size_t> textureCoords;
    std::unordered_map<FaceRaw, size_t> faces;

    // start with 'null' values
    hulls.emplace(nullptr, 0);
    normals.emplace(nullptr, 0);
    faces.emplace(nullptr, 0);
    edges.emplace(nullptr, 0);
    vertices.emplace(nullptr, 0);
    textureCoords.emplace(nullptr, 0);

    ForEachHull([&](const HullRaw& hull)
    {
        hulls.emplace(hull, hulls.size());
        hull->ForEachFace([&](const FaceRaw& face)
        {
            normals.emplace(face->GetNormal(), normals.size());
            faces.emplace(face, faces.size());
            face->ForEachEdge([&](const EdgeRaw& edge)
            {
                edges.emplace(edge, edges.size());
                vertices.emplace(edge->GetStartVertex(), vertices.size());
                normals.emplace(edge->GetStartNormal(), normals.size());
                textureCoords.emplace(edge->GetStartTextureCoord(), textureCoords.size());
            });
        });
    });

    // All work within one transaction
    db.BeginTransaction();

    // prepare for storing BoundingShapes
    db.ExecDML("DROP TABLE IF EXISTS BoundingShapes");
    db.ExecDML("CREATE TABLE BoundingShapes(Id INTEGER PRIMARY KEY,Type INTEGER,x0 FLOAT,y0 FLOAT,z0 FLOAT,x1 FLOAT,y1 FLOAT,z1 FLOAT,Radius FLOAT,Optimal INTEGER) WITHOUT ROWID");
    db.ExecDML("INSERT INTO BoundingShapes(Id,Type,x0,y0,z0,x1,y1,z1,Radius,Optimal) VALUES(0,%1%,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",BoundingShapeType::Convert(BoundingShape3d::Type::Unknown));
    SQLite::Statement boundingShapeStatement = db.CompileStatement("INSERT INTO BoundingShapes(Id,Type,x0,y0,z0,x1,y1,z1,Radius,Optimal) VALUES(?1,?2,?3,?4,?5,?6,?7,?8,?9,?10)");
    int boundingShapeCount = 1;
    auto StoreBoundingShape = [&boundingShapeStatement,&boundingShapeCount](const BoundingShape3d& bs)
    {
        if (bs.GetType() == BoundingShape3d::Type::Unknown)
        {
            return 0;
        }
        boundingShapeStatement.Reset();
        boundingShapeStatement.Bind(1, boundingShapeCount);
        boundingShapeStatement.Bind(2, BoundingShapeType::Convert(bs.GetType()));
        Vertex p0;
        if (bs.GetType() == BoundingShape3d::Type::Ball)
        {
            p0 = bs.GetCenter();
            boundingShapeStatement.BindNull(6);
            boundingShapeStatement.BindNull(7);
            boundingShapeStatement.BindNull(8);
            boundingShapeStatement.Bind(9,bs.GetRadius());
        }
        else
        {
            p0 = bs.GetMin();
            Vertex p1 = bs.GetMax();
            boundingShapeStatement.Bind(6, p1[0]);
            boundingShapeStatement.Bind(7, p1[1]);
            boundingShapeStatement.Bind(8, p1[2]);
            boundingShapeStatement.BindNull(9);
        }
        boundingShapeStatement.Bind(3, p0[0]);
        boundingShapeStatement.Bind(4, p0[1]);
        boundingShapeStatement.Bind(5, p0[2]);
        boundingShapeStatement.Bind(10, bs.IsOptimal()?1:0);
        boundingShapeStatement.ExecDML();
        return boundingShapeCount++;
    };

    // Header
    db.ExecDML("DROP TABLE IF EXISTS Header");
    db.ExecDML("CREATE TABLE Header(Key TEXT PRIMARY KEY,Value) WITHOUT ROWID");
    db.ExecDML("INSERT INTO Header(Key,Value) VALUES('Type','Shape')");
    db.ExecDML("INSERT INTO Header(Key,Value) VALUES('Version',%1%)", SerializationVersion);

    // Normals
    StoreVectorMap(db, normals, "Normals");

    // Vertices
    StoreVectorMap(db, vertices, "Vertices");

    // TextureCoords
    StoreVectorMap(db, textureCoords, "TextureCoords");

    // Edges
    db.ExecDML("DROP TABLE IF EXISTS Edges");
    db.ExecDML("CREATE TABLE Edges(Id INTEGER PRIMARY KEY,Vertex INTEGER,Normal INTEGER,Face INTEGER,Twin INTEGER,Next INTEGER,Prev INTEGER,Color INTEGER,TextureCoord INTEGER) WITHOUT ROWID");
    SQLite::Statement s = db.CompileStatement("INSERT INTO Edges(Id,Vertex,Normal,Face,Twin,Next,Prev,Color,TextureCoord) VALUES(?1,?2,?3,?4,?5,?6,?7,?8,?9)");
    for (auto& edge : edges)
    {
        if (edge.first)
        {
            s.Reset();
            s.Bind(1, (int64_t)edge.second);
            s.Bind(2, (int64_t)vertices[edge.first->GetStartVertex()]);
            s.Bind(3, (int64_t)normals[edge.first->GetStartNormal()]);
            s.Bind(4, (int64_t)faces[edge.first->GetFace()]);
            s.Bind(5, (int64_t)edges[edge.first->GetTwin()]);
            s.Bind(6, (int64_t)edges[edge.first->GetNext()]);
            s.Bind(7, (int64_t)edges[edge.first->GetPrev()]); // not needed?
            s.Bind(8, (int64_t)(edge.first->GetStartColor()?edge.first->GetStartColor()->GetInt():-1));
            s.Bind(9, (int64_t)textureCoords[edge.first->GetStartTextureCoord()]);
            s.ExecDML();
        }
    }

    // Faces
    db.ExecDML("DROP TABLE IF EXISTS Faces");
    db.ExecDML("CREATE TABLE Faces(Id INTEGER PRIMARY KEY,Hull INTEGER,Normal INTEGER,Color INTEGER) WITHOUT ROWID");
    s = db.CompileStatement("INSERT INTO Faces(Id,Hull,Normal,Color) VALUES(?1,?2,?3,?4)");
    for (auto& face : faces)
    {
        if (face.first)
        {
            s.Reset();
            s.Bind(1, (int64_t)face.second);
            s.Bind(2, (int64_t)hulls[face.first->GetHull()]);
            s.Bind(3, (int64_t)normals[face.first->GetNormal()]);
            s.Bind(4, (int64_t)(face.first->GetColor()?face.first->GetColor()->GetInt():-1));
            s.ExecDML();
        }
    }

    // Hulls
    db.ExecDML("DROP TABLE IF EXISTS Hulls");
    db.ExecDML("CREATE TABLE Hulls(Id INTEGER PRIMARY KEY,Shape INTEGER,Orientation INTEGER,Color INTEGER,BoundingShape INTEGER) WITHOUT ROWID");
    s = db.CompileStatement("INSERT INTO Hulls(Id,Shape,Orientation,Color,BoundingShape) VALUES(?1,0,?2,?3,?4)");
    for (auto& hull : hulls)
    {
        if (hull.first)
        {
            s.Reset();
            s.Bind(1, (int64_t)hull.second);
            s.Bind(2, Orientation::Convert(hull.first->GetOrientation()));
            s.Bind(3, (int64_t)(hull.first->GetColor() ? hull.first->GetColor()->GetInt() : -1));
            s.Bind(4, StoreBoundingShape(hull.first->GetBoundingShape()));
            s.ExecDML();
        }
    }

    // Shape
    db.ExecDML("DROP TABLE IF EXISTS Shapes");
    db.ExecDML("CREATE TABLE Shapes(Id INTEGER PRIMARY KEY,BoundingShape INTEGER) WITHOUT ROWID");
    s = db.CompileStatement("INSERT INTO Shapes(Id,BoundingShape) VALUES(?1,?2)");
    s.Reset();
    s.Bind(1, (int64_t)0);
    s.Bind(2, StoreBoundingShape(GetBoundingShape()));
    s.ExecDML();

    // End transaction
    db.CommitTransaction();
}

void Shape::Retrieve(SQLite::DB& db)
{
    std::unordered_map<size_t,HullPtr> hulls;
    std::unordered_map<size_t,EdgePtr> edges;
    std::unordered_map<size_t,VertexPtr> vertices;
    std::unordered_map<size_t,NormalPtr> normals;
    std::unordered_map<size_t,TextureCoordPtr> textureCoords;
    std::unordered_map<size_t,FacePtr> faces;
    std::unordered_map<size_t,BoundingShape3d> boundingShapes;

    Clear();

    // All work within one transaction
    db.BeginTransaction();

    // Check Header
    string type = db.ExecSingleString("SELECT Value FROM Header WHERE Key='Type'");
    int64_t serializationVersion = db.ExecSingleInt64("SELECT Value FROM Header WHERE Key='Version'");
    // TODO: check type and serializationVersion

    // Keep track of assigned colors
    std::unordered_map<int64_t, ColorPtr> colors;
    colors.emplace(-1, nullptr);
    auto ColorFromId = [&colors](const int64_t id)
    {
        auto iter = colors.find(id);
        if (colors.end() == iter)
        {
            iter = colors.emplace(id, Construct<Color>((unsigned int)id)).first;
        }
        return iter->second;
    };

    // Retrieve BoundingShapes
    SQLite::Query query = db.ExecQuery("SELECT Id,Type,x0,y0,z0,x1,y1,z1,Radius,Optimal FROM BoundingShapes");
    for (; !query.IsEOF(); query.NextRow())
    {
        BoundingShape3d bs;
        int64_t Id = query.GetInt64Field(0);
        BoundingShape3d::Type type = BoundingShapeType::Convert(query.GetInt64Field(1));
        if (type != BoundingShape3d::Type::Unknown)
        {
            Vertex p0(query.GetFloatField(2), query.GetFloatField(3), query.GetFloatField(4));
            bool optimal = (0 != query.GetInt64Field(9));
            if (type == BoundingShape3d::Type::Ball)
            {
                double radius = query.GetFloatField(8);
                bs.Set(p0, radius, optimal);
            }
            else
            {
                Vertex p1(query.GetFloatField(5), query.GetFloatField(6), query.GetFloatField(7));
                bs.Set(p0, p1, optimal);
            }
        }       
        boundingShapes.emplace(Id, bs);
    }

    // Read vertices
    ReadVectorMap(db, vertices, "Vertices");

    // Read normals
    ReadVectorMap(db, normals, "Normals");

    // Read textureCoords
    ReadVectorMap(db, textureCoords, "TextureCoords");

    // Shape
    query = db.ExecQuery("SELECT Id,BoundingShape FROM Shapes");
    SetBoundingShape(boundingShapes[query.GetInt64Field(1)]);

    // Hulls
    query = db.ExecQuery("SELECT Id,Orientation,Color,BoundingShape FROM Hulls");
    for (; !query.IsEOF(); query.NextRow())
    {
        int64_t Id = query.GetInt64Field(0);
        auto hull = ConstructAndAddHull();
        hull->SetOrientation(Orientation::Convert(query.GetIntField(1)));
        hull->SetColor(ColorFromId(query.GetInt64Field(2)));
        hull->SetBoundingShape(boundingShapes[query.GetInt64Field(3)]);
        hulls.emplace(Id, hull);
    }

    // Faces
    query = db.ExecQuery("SELECT Id,Hull,Normal,Color FROM Faces");
    for (; !query.IsEOF(); query.NextRow())
    {
        int64_t Id = query.GetInt64Field(0);
        int64_t hullId = query.GetInt64Field(1);
        auto face = hulls[hullId]->ConstructAndAddFace();
        face->SetNormal(normals[query.GetInt64Field(2)]);
        face->SetColor(ColorFromId(query.GetInt64Field(3)));
        faces.emplace(Id, face);
    }

    // Edges   
    // First add the edges, then fill the relation with others
    query = db.ExecQuery("SELECT Id,Vertex,Normal,Face FROM Edges");
    for (; !query.IsEOF(); query.NextRow())
    {
        int64_t Id = query.GetInt64Field(0);
        auto& vertex = vertices[query.GetInt64Field(1)];
        auto& normal = normals[query.GetInt64Field(2)];
        int64_t faceId = query.GetInt64Field(3);
        auto edge = faces[faceId]->ConstructAndAddEdge(vertex,normal);
        edges.emplace(Id, edge);
    }
    query = db.ExecQuery("SELECT Id,Twin,Next,Prev,Color,TextureCoord FROM Edges");
    for (; !query.IsEOF(); query.NextRow())
    {
        auto& edge = edges[query.GetInt64Field(0)];
        auto& twin = edges[query.GetInt64Field(1)];
        auto& next = edges[query.GetInt64Field(2)];
        auto& prev = edges[query.GetInt64Field(3)];
        edge->SetTwin(twin);
        edge->SetNext(next);
        edge->SetPrev(prev);
        edge->SetStartColor(ColorFromId(query.GetInt64Field(4)));
        edge->SetStartTextureCoord(textureCoords[query.GetInt64Field(5)]);
    }

    // End transaction
    db.CommitTransaction();

    // check the result
    ForEachFace([](const FaceRaw& face) {face->CheckPointering(); });
}

double Shape::CalculateVolume() const
{
    double res = 0;
    ForEachHull([&res](const HullRaw hull) 
    {
        double hullVolume = hull->CalculateVolume();
        if (hull->GetOrientation() == Hull::Orientation::Outward)
        {
            res += hullVolume;
        }
        else
        {
            res -= hullVolume;
        }
    });
    return res;
}

void Shape::Add(ShapePtr & other)
{
    std::vector<HullPtr> A(GetHulls().begin(), GetHulls().end());
    std::vector<HullPtr> B(other->GetHulls().begin(), other->GetHulls().end());

    for (auto i0 = std::rbegin(A); i0 != std::rend(A); ++i0)
    {
        for (auto i1 = std::rbegin(B); i1 != std::rend(B); ++i1)
        {
            HullPtr res = (*i0)->Add(*i1);
            if (res)
            {
                *i0 = res;
                B.erase(std::next(i1).base());
            }
        }
    }

    if (B.size() < other->GetHulls().size())
    {
        for (auto i0 = std::rbegin(A); i0 != std::rend(A); ++i0)
        {
            for (auto i1 = std::next(i0); i1 != std::rend(A); ++i1)
            {
                HullPtr res = (*i0)->Add(*i1);
                if (res)
                {
                    *i0 = res;
                    A.erase(std::next(i1).base());
                }
            }
        }
    }

    SetHulls(A.begin(), A.end(), B.begin(), B.end());
}

void Shape::Subtract(ShapePtr & other)
{
    // TODO
}

