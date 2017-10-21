#include "Geometry.h"
using namespace std;
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

void Shape::ForEachHull(std::function<void(const HullPtr& hull)> func) const
{
    for (const HullPtr& hull : GetHulls()) 
    { 
        func(hull); 
    }
}

void Shape::ForEachPatch(std::function<void(const PatchPtr& patch)> func) const
{
    ForEachHull([func](const HullPtr& hull) 
    { 
        hull->ForEachPatch(func); 
    });
}

void Shape::ForEachFace(std::function<void(const FacePtr& facePtr)> func) const
{
    ForEachHull([func](const HullPtr& hull) 
    { 
        hull->ForEachFace(func); 
    });
}

void Shape::ForEachEdge(std::function<void(const EdgePtr& edgePtr)> func) const
{
    ForEachHull([func](const HullPtr& hull) 
    { 
        hull->ForEachEdge(func); 
    });
}

void Shape::ForEachVertex(std::function<void(const VertexPtr& vertexPtr)> func) const
{
    std::unordered_set<VertexPtr> vertices;
    ForEachFace([&vertices](const FacePtr& face)
    {
        face->ForEachVertex([&vertices](const VertexPtr& vertex)
        {
            vertices.insert(vertex);
        });
    });
    for (const VertexPtr& vertex : vertices)
    {
        func(vertex);
    }
}

void Shape::SplitTrianglesIn4()
{
    ForEachHull([](const HullPtr& hull) 
    {
        hull->SplitTrianglesIn4(); 
    });
}

void Shape::Triangulate()
{
    ForEachHull([](const HullPtr& hull) 
    {
        hull->Triangulate(); 
    });
}

void Shape::Clear()
{
    m_hulls.clear();
}

void Shape::Scale(const double factor)
{
    ForEachVertex([factor](const VertexPtr& vertex)
    {
        (*vertex) *= factor;
    });
}

void Shape::Translate(const Vector3d& translation)
{
    ForEachVertex([translation](const VertexPtr& vertex)
    {
        (*vertex) += translation;
    });
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

void Shape::Store(SQLite::DB& db) const
{
    ForEachFace([](const FacePtr& face) {face->CheckPointering(); });

    std::unordered_map<HullPtr, size_t> hulls;
    std::unordered_map<EdgePtr, size_t> edges;
    std::unordered_map<VertexPtr, size_t> vertices;
    std::unordered_map<NormalPtr, size_t> normals;
    std::unordered_map<TextureCoordPtr, size_t> textureCoords;
    std::unordered_map<FacePtr, size_t> faces;
    std::unordered_map<PatchPtr, size_t> patches;

    std::unordered_map<const Hull*, size_t> hullsRaw;
    std::unordered_map<const Patch*, size_t> patchesRaw;

    // start with 'null' values
    hulls.emplace(nullptr, 0);
    patches.emplace(nullptr, 0);
    normals.emplace(nullptr, 0);
    faces.emplace(nullptr, 0);
    edges.emplace(nullptr, 0);
    vertices.emplace(nullptr, 0);
    textureCoords.emplace(nullptr, 0);

    ForEachHull([&](const HullPtr& hull)
    {
        hulls.emplace(hull, hulls.size());
        hull->ForEachPatch([&](const PatchPtr& patch)
        {
            patches.emplace(patch, patches.size());
            patch->ForEachFace([&](const FacePtr& face)
            {
                normals.emplace(face->GetNormal(), normals.size());
                faces.emplace(face, faces.size());
                face->ForEachEdge([&](const EdgePtr& edge)
                {
                    edges.emplace(edge, edges.size());
                    vertices.emplace(edge->GetStartVertex(), vertices.size());
                    normals.emplace(edge->GetStartNormal(), normals.size());
                    textureCoords.emplace(edge->GetStartTextureCoord(), textureCoords.size());
                });
            });
        });
    });

    // get raw ptr maps
    for (auto& item : patches)
    {
        patchesRaw.emplace(item.first ? item.first.get() : nullptr, item.second);
    }
    for (auto& item : hulls)
    {
        hullsRaw.emplace(item.first ? item.first.get() : nullptr, item.second);
    }

    // All work within one transaction
    db.BeginTransaction();

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
    db.ExecDML("CREATE TABLE Edges(Id TEXT PRIMARY KEY,Vertex INTEGER,Normal INTEGER,Face INTEGER,Twin INTEGER,Next INTEGER,Prev INTEGER,Color INTEGER,TextureCoord INTEGER) WITHOUT ROWID");
    SQLite::Statement statEdge = db.CompileStatement("INSERT INTO Edges(Id,Vertex,Normal,Face,Twin,Next,Prev,Color,TextureCoord) VALUES(?1,?2,?3,?4,?5,?6,?7,?8,?9)");
    for (auto& edge : edges)
    {
        if (edge.first)
        {
            statEdge.Reset();
            statEdge.Bind(1, (int64_t)edge.second);
            statEdge.Bind(2, (int64_t)vertices[edge.first->GetStartVertex()]);
            statEdge.Bind(3, (int64_t)normals[edge.first->GetStartNormal()]);
            statEdge.Bind(4, (int64_t)faces[edge.first->GetFace()]);
            statEdge.Bind(5, (int64_t)edges[edge.first->GetTwin()]);
            statEdge.Bind(6, (int64_t)edges[edge.first->GetNext()]);
            statEdge.Bind(7, (int64_t)edges[edge.first->GetPrev()]); // not needed?
            statEdge.Bind(8, (int64_t)(edge.first->GetStartColor()?edge.first->GetStartColor()->GetInt():-1));
            statEdge.Bind(9, (int64_t)textureCoords[edge.first->GetStartTextureCoord()]);
            statEdge.ExecDML();
        }
    }

    // Faces
    db.ExecDML("DROP TABLE IF EXISTS Faces");
    db.ExecDML("CREATE TABLE Faces(Id TEXT PRIMARY KEY,Patch INTEGER,Normal INTEGER,Color INTEGER) WITHOUT ROWID");
    SQLite::Statement statFace = db.CompileStatement("INSERT INTO Faces(Id,Patch,Normal,Color) VALUES(?1,?2,?3,?4)");
    for (auto& face : faces)
    {
        if (face.first)
        {
            statFace.Reset();
            statFace.Bind(1, (int64_t)face.second);
            statFace.Bind(2, (int64_t)patchesRaw[&face.first->GetPatch()]);
            statFace.Bind(3, (int64_t)normals[face.first->GetNormal()]);
            statFace.Bind(4, (int64_t)(face.first->GetColor()?face.first->GetColor()->GetInt():-1));
            statFace.ExecDML();
        }
    }

    // Patches
    // TODO: textureId? boundingShape?
    db.ExecDML("DROP TABLE IF EXISTS Patches");
    db.ExecDML("CREATE TABLE Patches(Id TEXT PRIMARY KEY,Hull INTEGER,Color INTEGER) WITHOUT ROWID");
    SQLite::Statement statPatch = db.CompileStatement("INSERT INTO Patches(Id,Hull,Color) VALUES(?1,?2,?3)");
    for (auto& patch : patches)
    {
        if (patch.first)
        {
            statPatch.Reset();
            statPatch.Bind(1, (int64_t)patch.second);
            statPatch.Bind(2, (int64_t)hullsRaw[&patch.first->GetHull()]);
            statPatch.Bind(3, (int64_t)(patch.first->GetColor()?patch.first->GetColor()->GetInt():-1));
            statPatch.ExecDML();
        }
    }

    // Hulls
    // TODO: boundingShape?
    db.ExecDML("DROP TABLE IF EXISTS Hulls");
    db.ExecDML("CREATE TABLE Hulls(Id TEXT PRIMARY KEY,Orientation INTEGER) WITHOUT ROWID");
    SQLite::Statement statHull = db.CompileStatement("INSERT INTO Hulls(Id,Orientation) VALUES(?1,?2)");
    for (auto& hull : hulls)
    {
        if (hull.first)
        {
            statHull.Reset();
            statHull.Bind(1, (int64_t)hull.second);
            statHull.Bind(2, (int64_t)hull.first->GetOrientation());
            statHull.ExecDML();
        }
    }

    // Shape
    // TODO: boundingShape?

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
    std::unordered_map<size_t,PatchPtr> patches;

    Clear();

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

    // Read vertices
    ReadVectorMap(db, vertices, "Vertices");

    // Read normals
    ReadVectorMap(db, normals, "Normals");

    // Read textureCoords
    ReadVectorMap(db, textureCoords, "TextureCoords");

    // Shape

    // Hulls
    SQLite::Query query = db.ExecQuery("SELECT Id,Orientation FROM Hulls");
    for (; !query.IsEOF(); query.NextRow())
    {
        int64_t Id = query.GetInt64Field(0);
        auto hull = ConstructAndAddHull();
        hull->SetOrientation((Hull::Orientation)query.GetIntField(1));
        hulls.emplace(Id, hull);
    }

    // Patches
    query = db.ExecQuery("SELECT Id,Hull,Color FROM Patches");
    for (; !query.IsEOF(); query.NextRow())
    {
        int64_t Id = query.GetInt64Field(0);
        int64_t hullId = query.GetInt64Field(1);
        auto patch = hulls[hullId]->ConstructAndAddPatch();
        patch->SetColor(ColorFromId(query.GetInt64Field(2)));
        patches.emplace(Id, patch);
    }

    // Faces
    query = db.ExecQuery("SELECT Id,Patch,Normal,Color FROM Faces");
    for (; !query.IsEOF(); query.NextRow())
    {
        int64_t Id = query.GetInt64Field(0);
        int64_t patchId = query.GetInt64Field(1);
        auto face = patches[patchId]->ConstructAndAddFace();
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
        auto edge = Face::ConstructAndAddEdge(faces[faceId],vertex,normal);
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

    // check the result
    ForEachFace([](const FacePtr& face) {face->CheckPointering(); });
}
