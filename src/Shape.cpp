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


////
//// keep write/read next to each-other for ease of coding
//// exceptions indicate status, log details to logger
////
//namespace Serialization
//{
//    namespace Unsigned
//    {
//        void Write(Common::Serializer& s, const unsigned int value, const unsigned int bits)
//        {
//            s.Write(value, bits);
//        }
//        unsigned int Read(Common::Serializer& s, const unsigned int bits)
//        {
//            return s.Read<unsigned int>(bits);
//        }
//        void Test(unsigned int expected,unsigned int res, const std::string& message)
//        {
//            if (res != expected)
//            {
//                throw std::runtime_error(message);
//            }
//        }
//        unsigned int ReadAndTest(Common::Serializer& s, const unsigned int expected, const unsigned int bits, const std::string& message)
//        {
//            unsigned int res = s.Read<unsigned int>(bits);
//            Test(expected, res, message);
//            return res;
//        }
//    }
//
//    class Index
//    {
//    public:
//        Index()
//            : m_index(0)
//        {}
//        Index(unsigned int index, unsigned int size)
//            : m_index(index)
//            , m_size(size)
//        {}
//
//        void Write(Common::Serializer& s) const { Unsigned::Write(s, m_index, m_size); }
//        void Read(Common::Serializer& s) { m_index = Unsigned::Read(s, m_size); }
//
//        unsigned int GetIndex() const { return m_index; }
//        unsigned int GetSize() const { return m_size; }
//
//        void SetIndex(unsigned int index) { m_index = index; }
//        void SetSize(unsigned int size) { m_size = size; }
//
//        operator unsigned int() const { return GetIndex(); }
//
//        template<typename CONTAINER, typename ITEM>
//        static Index FindIndex(const CONTAINER& c, const ITEM& i) 
//        {
//            Index index;
//            auto iter = c.find(i);
//            if (iter == c.end())
//            {
//                assert(false); // item in object not in list of items?
//                iter = c.find(nullptr);
//            }
//            index = iter->second;
//            return index;
//        }
//
//        template<typename INDEX_CONTAINER, typename ITEM_CONTAINER>
//        static std::pair<unsigned int, std::vector<Index>> FindIndices(const INDEX_CONTAINER& indices, const ITEM_CONTAINER& items)
//        {
//            std::pair<unsigned int, std::vector<Index>> res;
//            res.first = Index::CalcSize(indices.size());
//            res.second.reserve(items.size());
//            for (const auto& item : items)
//            {
//                res.second.emplace_back(FindIndex(indices,item));
//                assert(res.first == res.second.back().GetSize());
//            }
//            return res;
//        }
//
//        static unsigned int CalcSize(unsigned int count)
//        {
//            // this is a very naive way to find the highest bit.
//            unsigned int s = 0; 
//            do
//            {
//                s += 1;
//                count /= 2;
//            } 
//            while (count > 0);
//            assert(count == 0);
//            return s;
//        }
//
//    private:
//        unsigned int m_index;
//        unsigned int m_size;
//    };
//
//    // pointer tracing
//    struct PointerInfo
//    {
//        std::map<HullPtr::Raw, Index> hulls;
//        std::map<EdgePtr::Raw, Index> edges;
//        std::map<VertexPtr::Raw, Index> vertices;
//        std::map<NormalPtr::Raw, Index> normals;
//        std::map<TextureCoordPtr::Raw, Index> textureCoords;
//        std::map<ColorPtr::Raw, Index> colors;
//        std::map<FacePtr::Raw, Index> faces;
//        std::map<PatchPtr::Raw, Index> patches;
//
//        PointerInfo(const ShapePtr::Raw& shape)
//        {
//            shape->ForEachHull([&](const HullPtr::Raw& hull)
//            {
//                hulls.emplace(hull, Index());
//                hull->ForEachPatch([&](const PatchPtr::Raw& patch)
//                {
//                    colors.emplace(patch->GetColor(), Index());
//                    patches.emplace(patch, Index());
//                    patch->ForEachFace([&](const FacePtr::Raw& face)
//                    {
//                        normals.emplace(face->GetNormal(), Index());
//                        colors.emplace(face->GetColor(), Index());
//                        faces.emplace(face, Index());
//                        face->ForEachEdge([&](const EdgePtr::Raw& edge)
//                        {
//                            edges.emplace(edge, Index());
//                            vertices.emplace(edge->GetStartVertex(), Index());
//                            normals.emplace(edge->GetStartNormal(), Index());
//                            colors.emplace(edge->GetStartColor(), Index());
//                            textureCoords.emplace(edge->GetStartTextureCoord(), Index());
//                        });
//                    });
//                });
//            });
//
//            FillIndex(hulls);
//            FillIndex(edges);
//            FillIndex(vertices);
//            FillIndex(normals);
//            FillIndex(textureCoords);
//            FillIndex(colors);
//            FillIndex(faces);
//            FillIndex(patches);
//        }
//
//    private:
//        template<typename T>
//        void FillIndex(T& t)
//        {
//            t.erase(nullptr);
//            unsigned int i = 1;
//            unsigned int s = Index::CalcSize(t.size());
//            for (auto& kv : t)
//            {
//                kv.second = Index(i++,s);
//            }
//            t.emplace(nullptr, Index(0, s));
//        };
//    };
//
//    namespace Base
//    {
//        template<typename EImpl>
//        struct Entity
//        {
//            template<typename... Args>
//            void Write(Common::Serializer& s, const Args&... args)
//            {
//                WriteInit();
//                WriteCore(s, args...);
//                WriteFinish();
//            }
//            template<typename... Args>
//            void Read(Common::Serializer& s, Args&... args)
//            {
//                ReadCore(s, args...);
//            }
//        private:
//            void WriteInit()
//            {}
//            void WriteFinish()
//            {}
//            template<typename T,typename... Args>
//            void WriteCore(Common::Serializer& s, const T& t, const Args&... args)
//            {
//                WriteItem(s, t);
//                WriteCore(s, args...);
//            }
//            void WriteCore(Common::Serializer& s)
//            {}
//
//            template<typename T, typename... Args>
//            void ReadCore(Common::Serializer& s, T& t, Args&... args)
//            {
//                ReadItem(s, t);
//                ReadCore(s, args...);
//            }
//            void ReadCore(Common::Serializer& s)
//            {}
//
//            void WriteItem(Common::Serializer& s, const Index& index)
//            {
//                index.Write(s);
//            }
//            void ReadItem(Common::Serializer& s, Index& index)
//            {
//                index.Read(s);
//            }
//
//            void WriteItem(Common::Serializer& s, const std::pair<unsigned int,std::vector<Index>>& indices)
//            {
//                s.Write(indices.second.size(), 24);
//                for (const auto& index : indices.second)
//                {
//                    index.Write(s);
//                }
//            }
//            void ReadItem(Common::Serializer& s, std::pair<unsigned int,std::vector<Index>>& indices)
//            {
//                indices.second.clear();
//                unsigned int size = Unsigned::Read(s, 24);
//                unsigned int bits = indices.first;
//                indices.second.resize(size, Index(0, bits));
//                for (auto& index : indices.second)
//                {
//                    index.Read(s);
//                }
//            }
//
//            void WriteItem(Common::Serializer& s, const BoundingShape3d& bs)
//            {
//                bs.Serialize(s);
//            }
//            void ReadItem(Common::Serializer& s, BoundingShape3d& bs)
//            {
//                bs.Deserialize(s);
//            }
//
//            void WriteItem(Common::Serializer& s, const Geometry::Hull::Orientation& orientation)
//            {
//                s.Write((unsigned char)orientation);
//            }
//            void ReadItem(Common::Serializer& s, Geometry::Hull::Orientation& orientation)
//            {
//                unsigned int o = Unsigned::Read(s, 8);
//                orientation = (Geometry::Hull::Orientation)o;
//            }
//        };
//    }
//
//    struct PointerInfoIndex;
//
//    namespace Hull
//    {
//        struct Entity : public Base::Entity<Hull::Entity>
//        {
//            typedef Base::Entity<Hull::Entity> MyBase;
//
//            Geometry::Hull::Orientation orientation;
//            BoundingShape3d bs;
//            std::pair<unsigned int,std::vector<Index>> patches;
//
//            Entity(const Geometry::Hull& hull, const PointerInfo& pi)
//            {
//                orientation = hull.GetOrientation();
//                bs = hull.GetBoundingShape();
//                patches = Index::FindIndices(pi.patches, hull.GetPatches());
//            }
//            Entity(const PointerInfoIndex& pii);
//            Entity()
//            {}
//
//            void Write(Common::Serializer& s)
//            {
//                MyBase::Write(s, orientation, bs, patches);
//            }
//            void Read(Common::Serializer& s)
//            {
//                MyBase::Read(s, orientation, bs, patches);
//            }
//        };
//    }
//    namespace Patch
//    {
//        struct Entity : public Base::Entity<Patch::Entity>
//        {
//            typedef Base::Entity<Patch::Entity> MyBase;
//
//            Index color;
//            BoundingShape3d bs;
//            std::pair<unsigned int, std::vector<Index>> faces;
//
//            Entity(const Geometry::Patch& patch, const PointerInfo& pi)
//            {
//                color = Index::FindIndex(pi.colors, patch.GetColor());
//                bs = patch.GetBoundingShape();
//                faces = Index::FindIndices(pi.faces, patch.GetFaces());
//            }
//            Entity(const PointerInfoIndex& pii);
//            Entity()
//            {}
//
//            void Write(Common::Serializer& s)
//            {
//                MyBase::Write(s, color, bs, faces);
//            }
//            void Read(Common::Serializer& s)
//            {
//                MyBase::Read(s, color, bs, faces);
//            }
//        };
//    }
//    namespace Face
//    {
//        struct Entity : public Base::Entity<Face::Entity>
//        {
//            typedef Base::Entity<Face::Entity> MyBase;
//
//            Index color;
//            std::pair<unsigned int, std::vector<Index>> edges;
//
//            Entity(const Geometry::Face& face, const PointerInfo& pi)
//            {
//                color = Index::FindIndex(pi.colors, face.GetColor());
//                edges = Index::FindIndices(pi.edges, face.GetSharedEdgesUnordered());
//            }
//            Entity(const PointerInfoIndex& pii);
//            Entity()
//            {}
//
//            void Write(Common::Serializer& s)
//            {
//                MyBase::Write(s, color, edges);
//            }
//            void Read(Common::Serializer& s)
//            {
//                MyBase::Read(s, color, edges);
//            }
//        };
//    }
//    namespace Edge
//    {
//        struct Entity : public Base::Entity<Edge::Entity>
//        {
//            typedef Base::Entity<Edge::Entity> MyBase;
//
//            Index color;
//            Index textureCoord;
//            Index vertex;
//            Index normal;
//            Index twin;
//            Index next;
//            Index prev; // not needed (can be recalculated)?
//
//            Entity(const Geometry::Edge& edge, const PointerInfo& pi)
//            {
//                color = Index::FindIndex(pi.colors, edge.GetStartColor());
//                textureCoord = Index::FindIndex(pi.textureCoords, edge.GetStartTextureCoord());
//                vertex = Index::FindIndex(pi.vertices, edge.GetStartVertex());
//                normal = Index::FindIndex(pi.normals, edge.GetStartNormal());
//                twin = Index::FindIndex(pi.edges, edge.GetTwin());
//                next = Index::FindIndex(pi.edges, edge.GetNext());
//                prev = Index::FindIndex(pi.edges, edge.GetPrev());
//            }
//            Entity(const PointerInfoIndex& pii);
//            Entity()
//            {}
//
//            void Write(Common::Serializer& s)
//            {
//                MyBase::Write(s, color, textureCoord, vertex, normal, twin, next, prev);
//            }
//            void Read(Common::Serializer& s)
//            {
//                MyBase::Read(s, color, textureCoord, vertex, normal, twin, next, prev);
//            }
//        };
//    }
//
//    // pointer tracing
//    struct PointerInfoIndex
//    {
//        std::vector<VertexPtr::Shared> vertices;
//        std::vector<NormalPtr::Shared> normals;
//        std::vector<TextureCoordPtr::Shared> textureCoords;
//        std::vector<ColorPtr::Shared> colors;
//        std::vector<std::pair<HullPtr::Shared, Serialization::Hull::Entity>> hulls;
//        std::vector<std::pair<EdgePtr::Shared, Serialization::Edge::Entity>> edges;
//        std::vector<std::pair<FacePtr::Shared, Serialization::Face::Entity>> faces;
//        std::vector<std::pair<PatchPtr::Shared, Serialization::Patch::Entity>> patches;
//
//        PointerInfoIndex()
//        {
//            vertices.emplace_back(nullptr);
//            normals.emplace_back(nullptr);
//            textureCoords.emplace_back(nullptr);
//            colors.emplace_back(nullptr);
//            hulls.emplace_back(nullptr, Serialization::Hull::Entity());
//            edges.emplace_back(nullptr, Serialization::Edge::Entity());
//            faces.emplace_back(nullptr, Serialization::Face::Entity());
//            patches.emplace_back(nullptr, Serialization::Patch::Entity());
//        }
//    };
//
//    namespace Header
//    {
//        static const std::string msg_magic_nr_mismatch = "Magic nr mismatch. Is this really Shape data?";
//        static const std::string msg_version_nr_mismatch = "Shape data version mismatch";
//
//        void Write(Common::Serializer& s)
//        {
//            s.Write(SerializationMagicNr, 32);
//            s.Write(SerializationVersion, 16);
//        }
//        void Write(Common::DB& db)
//        {
//            db.execDML("CREATE TABLE HEADER(Key TEXT PRIMARY KEY,Value) WITHOUT ROWID");
//            db.execDML("INSERT INTO HEADER(Key,Value) VALUES('Version',%1%)", SerializationVersion);
//        }
//        void Read(Common::Serializer& s)
//        {
//            Serialization::Unsigned::ReadAndTest(s, SerializationMagicNr, 32, msg_magic_nr_mismatch);
//            Serialization::Unsigned::ReadAndTest(s, SerializationVersion, 16, msg_version_nr_mismatch);
//        }
//        void Read(Common::DB& db)
//        {
//            Serialization::Unsigned::Test(SerializationVersion, db.execSingleInt("SELECT Value FROM HEADER WHERE Key='Version';"), msg_version_nr_mismatch);
//        }
//    }
//
//    namespace Colors
//    {
//        void Write(Common::Serializer& s, const PointerInfo& pi)
//        {
//            s.Write(pi.colors.size(), 16);
//            for (const auto& color : pi.colors)
//            {
//                if (color.first != nullptr)
//                {
//                    color.first->Serialize(s);
//                }
//            }
//        }
//        void Write(Common::DB& db, const PointerInfo& pi)
//        {
//            db.execDML("CREATE TABLE Colors(Id INTEGER PRIMARY KEY, RGB INT) WITHOUT ROWID");
//            auto stm = db.compileStatement("INSERT INTO Colors(Id,RGB) VALUES(?1,?2)");
//            for (const auto& color : pi.colors)
//            {
//                if (color.first != nullptr)
//                {
//                    stm.bind(1, (int)color.second);
//                    stm.bind(2, (int64_t)color.first->GetInt());
//                    stm.execDML();
//                }
//            }
//        }
//        void Read(Common::Serializer& s, std::vector<ColorPtr::Shared>& colors)
//        {
//            unsigned int count;
//            s.Read(count, 16);
//            colors.clear();
//            colors.resize(count);
//            for (unsigned int i = 1; i < count; ++i)
//            {
//                colors[i] = std::make_shared<Color>(Color::Deserialize(s));
//            }
//        }
//        void Read(Common::DB& db, std::vector<ColorPtr::Shared>& colors)
//        {
//            unsigned int count = db.execSingleInt("SELECT MAX(Id) FROM Colors;");
//            colors.clear();
//            colors.resize(count+1);
//            auto query = db.execQuery("SELECT Id,RGB FROM Colors;");
//            while (!query.eof())
//            {
//                colors[query.getIntField(0)] = std::make_shared<Color>((unsigned int)query.getInt64Field(1));
//                query.nextRow();
//            }
//        }
//    }
//
//    namespace TextureCoords
//    {
//        void Write(Common::Serializer& s, const PointerInfo& pi)
//        {
//            s.Write(pi.textureCoords.size(), 16);
//            for (const auto& textureCoord : pi.textureCoords)
//            {
//                if (textureCoord.first != nullptr)
//                {
//                    textureCoord.first->Serialize(s);
//                }
//            }
//        }
//        void Write(Common::DB& db, const PointerInfo& pi)
//        {
//            db.execDML("CREATE TABLE TextureCoordinates(Id INTEGER PRIMARY KEY, X FLOAT, Y FLOAT) WITHOUT ROWID");
//            auto stm = db.compileStatement("INSERT INTO TextureCoordinates(Id,X,Y) VALUES(?1,?2,?3)");
//            for (const auto& textureCoord : pi.textureCoords)
//            {
//                if (textureCoord.first != nullptr)
//                {
//                    stm.bind(1, (int)textureCoord.second);
//                    stm.bind(2, textureCoord.first->operator[](0));
//                    stm.bind(3, textureCoord.first->operator[](1));
//                    stm.execDML();
//                }
//            }
//        }
//        void Read(Common::Serializer& s, std::vector<TextureCoordPtr::Shared>& textureCoords)
//        {
//            unsigned int count;
//            s.Read(count, 16);
//            textureCoords.clear();
//            textureCoords.resize(count);
//            for (unsigned int i = 1; i<count; ++i)
//            {
//                textureCoords[i] = std::make_shared<TextureCoord>(TextureCoord::Deserialize(s));
//            }
//        }
//        void Read(Common::DB& db, std::vector<TextureCoordPtr::Shared>& textureCoords)
//        {
//            unsigned int count = db.execSingleInt("SELECT MAX(Id) FROM TextureCoordinates;");
//            textureCoords.clear();
//            textureCoords.resize(count + 1);
//            auto query = db.execQuery("SELECT Id,X,Y FROM TextureCoordinates;");
//            while (!query.eof())
//            {
//                textureCoords[query.getIntField(0)] = std::make_shared<TextureCoord>(query.getFloatField(1), query.getFloatField(2));
//                query.nextRow();
//            }
//        }
//    }
//
//    namespace Normals
//    {
//        void Write(Common::Serializer& s, const PointerInfo& pi)
//        {
//            s.Write(pi.normals.size(), 16);
//            for (const auto& normal : pi.normals)
//            {
//                if (normal.first != nullptr)
//                {
//                    normal.first->Serialize(s);
//                }
//            }
//        }
//        void Write(Common::DB& db, const PointerInfo& pi)
//        {
//            db.execDML("CREATE TABLE Normals(Id INTEGER PRIMARY KEY, U FLOAT, V FLOAT, W FLOAT) WITHOUT ROWID");
//            auto stm = db.compileStatement("INSERT INTO Normals(Id,U,V,W) VALUES(?1,?2,?3,?4)");
//            for (const auto& normal : pi.normals)
//            {
//                if (normal.first != nullptr)
//                {
//                    stm.bind(1, (int)normal.second);
//                    stm.bind(2, normal.first->operator[](0));
//                    stm.bind(3, normal.first->operator[](1));
//                    stm.bind(4, normal.first->operator[](2));
//                    stm.execDML();
//                }
//            }
//        }
//        void Read(Common::Serializer& s, std::vector<NormalPtr::Shared>& normals)
//        {
//            unsigned int count;
//            s.Read(count, 16);
//            normals.clear();
//            normals.resize(count);
//            for (unsigned int i = 1; i<count; ++i)
//            {
//                normals[i] = std::make_shared<Normal>(Normal::Deserialize(s));
//            }
//        }
//        void Read(Common::DB& db, std::vector<NormalPtr::Shared>& normals)
//        {
//            unsigned int count = db.execSingleInt("SELECT MAX(Id) FROM Normals;");
//            normals.clear();
//            normals.resize(count + 1);
//            auto query = db.execQuery("SELECT Id,U,V,W FROM Normals;");
//            while (!query.eof())
//            {
//                normals[query.getIntField(0)] = std::make_shared<Normal>(query.getFloatField(1), query.getFloatField(2), query.getFloatField(3));
//                query.nextRow();
//            }
//        }
//    }
//
//    namespace Vertices
//    {
//        void Write(Common::Serializer& s, const PointerInfo& pi)
//        {
//            s.Write(pi.vertices.size(), 16);
//            for (const auto& vertex : pi.vertices)
//            {
//                if (vertex.first != nullptr)
//                {
//                    vertex.first->Serialize(s);
//                }
//            }
//        }
//        void Write(Common::DB& db, const PointerInfo& pi)
//        {
//            db.execDML("CREATE TABLE Vertices(Id INTEGER PRIMARY KEY, X FLOAT, Y FLOAT, Z FLOAT) WITHOUT ROWID");
//            auto stm = db.compileStatement("INSERT INTO Vertices(Id,X,Y,Z) VALUES(?1,?2,?3,?4)");
//            for (const auto& vertex : pi.vertices)
//            {
//                if (vertex.first != nullptr)
//                {
//                    stm.bind(1, (int)vertex.second);
//                    stm.bind(2, vertex.first->operator[](0));
//                    stm.bind(3, vertex.first->operator[](1));
//                    stm.bind(4, vertex.first->operator[](2));
//                    stm.execDML();
//                }
//            }
//        }
//        void Read(Common::Serializer& s, std::vector<VertexPtr::Shared>& vertices)
//        {
//            unsigned int count;
//            s.Read(count, 16);
//            vertices.clear();
//            vertices.resize(count);
//            for (unsigned int i = 1; i<count; ++i)
//            {
//                vertices[i] = std::make_shared<Vertex>(Vertex::Deserialize(s));
//            }
//        }
//        void Read(Common::DB& db, std::vector<VertexPtr::Shared>& vertices)
//        {
//            unsigned int count = db.execSingleInt("SELECT MAX(Id) FROM Vertices;");
//            vertices.clear();
//            vertices.resize(count + 1);
//            auto query = db.execQuery("SELECT Id,X,Y,Z FROM Vertices;");
//            while (!query.eof())
//            {
//                vertices[query.getIntField(0)] = std::make_shared<Vertex>(query.getFloatField(1), query.getFloatField(2), query.getFloatField(3));
//                query.nextRow();
//            }
//        }
//    }
//
//    namespace Hull
//    {
//        Entity::Entity(const PointerInfoIndex& pii)
//        {
//            patches.first = Index::CalcSize(pii.patches.size());
//        }
//        void Write(Common::Serializer& s, const PointerInfo& pi, const Geometry::Hull& hull)
//        {
//            Hull::Entity entity(hull,pi);
//            entity.Write(s);
//        }
//        void Read(Common::Serializer& s, PointerInfoIndex& pii, const unsigned int i)
//        {
//            Hull::Entity entity(pii);
//            entity.Read(s);
//            pii.hulls[i] = std::make_pair(Geometry::Hull::Construct(), entity);
//        }
//        void ApplyInfo(PointerInfoIndex& pii)
//        {
//            for (auto& hi : pii.hulls)
//            {
//                if (hi.first)
//                {
//                    HullPtr::Shared & hull = hi.first;
//                    Hull::Entity & entity = hi.second;
//                    hull->SetOrientation(entity.orientation);
//                    hull->SetBoundingShape(entity.bs);
//                    for (const Index& patchIndex : entity.patches.second)
//                    {
//                        PatchPtr::Shared& pp = pii.patches[patchIndex].first;
//                        assert(pp);
//                        hull->AddPatch(pp);
//                        pp->SetHull(hull);
//                    }
//                }
//            }
//        }
//    }
//
//    namespace Patch
//    {
//        Entity::Entity(const PointerInfoIndex& pii)
//        {
//            color.SetSize(Index::CalcSize(pii.colors.size()));
//            faces.first = Index::CalcSize(pii.faces.size());
//        }
//        void Write(Common::Serializer& s, const PointerInfo& pi, const Geometry::Patch& patch)
//        {
//            Patch::Entity entity(patch, pi);
//            entity.Write(s);
//        }
//        void Read(Common::Serializer& s, PointerInfoIndex& pii, const unsigned int i)
//        {
//            Patch::Entity entity(pii);
//            entity.Read(s);
//            pii.patches[i] = std::make_pair(Geometry::Patch::Construct(), entity);
//        }
//        void ApplyInfo(PointerInfoIndex& pii)
//        {
//            for (auto& pi : pii.patches)
//            {
//                if (pi.first)
//                {
//                    PatchPtr::Shared & patch = pi.first;
//                    Patch::Entity & entity = pi.second;
//                    patch->SetBoundingShape(entity.bs);
//                    patch->SetColor(pii.colors[entity.color]);
//                    for (const Index& faceIndex : entity.faces.second)
//                    {
//                        FacePtr::Shared& fp = pii.faces[faceIndex].first;
//                        patch->AddFace(fp);
//                        assert(fp);
//                        fp->SetPatch(patch);
//                    }
//                    patch->SetDisplayList(0);
//                    patch->SetTextureId(0);
//                }
//            }
//        }
//    }
//
//    namespace Face
//    {
//        Entity::Entity(const PointerInfoIndex& pii)
//        {
//            color.SetSize(Index::CalcSize(pii.colors.size()));
//            edges.first = Index::CalcSize(pii.edges.size());
//        }
//        void Write(Common::Serializer& s, const PointerInfo& pi, const Geometry::Face& face)
//        {
//            Face::Entity entity(face, pi);
//            entity.Write(s);
//        }
//        void Read(Common::Serializer& s, PointerInfoIndex& pii, const unsigned int i)
//        {
//            Face::Entity entity(pii);
//            entity.Read(s);
//            pii.faces[i] = std::make_pair(Geometry::Face::Construct(), entity);
//        }
//        void ApplyInfo(PointerInfoIndex& pii)
//        {
//            for (auto& fi : pii.faces)
//            {
//                if (fi.first)
//                {
//                    FacePtr::Shared & face = fi.first;
//                    Face::Entity & entity = fi.second;
//                    face->SetColor(pii.colors[entity.color]);
//                    for (const Index& edgeIndex : entity.edges.second)
//                    {
//                        EdgePtr::Shared& ep = pii.edges[edgeIndex].first;
//                        face->AddEdge(ep);
//                        assert(ep);
//                        ep->SetFace(face);
//                    }
//                    face->CalcNormal();
//                    face->CheckPointering();
//                }
//            }
//        }
//    }
//
//    namespace Edge
//    {
//        Entity::Entity(const PointerInfoIndex& pii)
//        {
//            color.SetSize(Index::CalcSize(pii.colors.size()));
//            textureCoord.SetSize(Index::CalcSize(pii.textureCoords.size()));
//            vertex.SetSize(Index::CalcSize(pii.vertices.size()));
//            normal.SetSize(Index::CalcSize(pii.normals.size()));
//            twin.SetSize(Index::CalcSize(pii.edges.size()));
//            next.SetSize(twin.GetSize());
//            prev.SetSize(twin.GetSize());
//        }
//        void Write(Common::Serializer& s, const PointerInfo& pi, const Geometry::Edge& edge)
//        {
//            Edge::Entity entity(edge, pi);
//            entity.Write(s);
//        }
//        void Read(Common::Serializer& s, PointerInfoIndex& pii, const unsigned int i)
//        {
//            Edge::Entity entity(pii);
//            entity.Read(s);
//            pii.edges[i] = std::make_pair(Geometry::Edge::Construct(), entity);
//        }
//        void ApplyInfo(PointerInfoIndex& pii)
//        {
//            for (auto& ei : pii.edges)
//            {
//                if (ei.first)
//                {
//                    EdgePtr::Shared & edge = ei.first;
//                    Edge::Entity & entity = ei.second;
//                    edge->SetStartColor(pii.colors[entity.color]);
//                    edge->SetStartTextureCoord(pii.textureCoords[entity.textureCoord]);
//                    edge->SetStartVertex(pii.vertices[entity.vertex]);
//                    edge->SetStartNormal(pii.normals[entity.normal]);
//                    edge->SetTwin(pii.edges[entity.twin].first);
//                    edge->SetNext(pii.edges[entity.next].first);
//                    edge->SetPrev(pii.edges[entity.prev].first);
//                }
//            }
//        }
//    }
//
//    namespace Shape
//    {
//        void Write(Common::Serializer& s, const PointerInfo& pi, const Geometry::Shape& shape)
//        {
//            // shape members
//            shape.GetBoundingShape().Serialize(s);
//
//            // write sizes
//            s.Write(pi.edges.size(), 24);
//            s.Write(pi.faces.size(), 24);
//            s.Write(pi.patches.size(), 24);
//            s.Write(pi.hulls.size(), 24);
//
//            // edges
//            for (const auto& edge : pi.edges)
//            {
//                if (edge.first != nullptr)
//                {
//                    Serialization::Edge::Write(s, pi, *edge.first);
//                }
//            }
//
//            // faces
//            for (const auto& face : pi.faces)
//            {
//                if (face.first != nullptr)
//                {
//                    Serialization::Face::Write(s, pi, *face.first);
//                }
//            }
//
//            // patches
//            for (const auto& patch : pi.patches)
//            {
//                if (patch.first != nullptr)
//                {
//                    Serialization::Patch::Write(s, pi, *patch.first);
//                }
//            }
//
//            // hulls
//            for (const auto& hull : pi.hulls)
//            {
//                if (hull.first != nullptr)
//                {
//                    Serialization::Hull::Write(s, pi, *hull.first);
//                }
//            }
//        }
//        void Read(Common::Serializer& s, PointerInfoIndex& pii, Geometry::Shape& shape)
//        {
//            // shape members
//            shape.SetBoundingShape(BoundingShape3d::Deserialize(s));
//
//            // sizes
//            unsigned int edgeCount = Unsigned::Read(s, 24);
//            unsigned int faceCount = Unsigned::Read(s, 24);
//            unsigned int patchCount = Unsigned::Read(s, 24);
//            unsigned int hullCount = Unsigned::Read(s, 24);
//
//            // create containers
//            pii.edges.resize(edgeCount);
//            pii.faces.resize(faceCount);
//            pii.patches.resize(patchCount);
//            pii.hulls.resize(hullCount);
//
//            // edges
//            for (unsigned int i = 1; i < edgeCount; ++i)
//            {
//                Serialization::Edge::Read(s, pii, i);
//            }
//
//            // faces
//            for (unsigned int i = 1; i < faceCount; ++i)
//            {
//                Serialization::Face::Read(s, pii, i);
//            }
//
//            // patches
//            for (unsigned int i = 1; i < patchCount; ++i)
//            {
//                Serialization::Patch::Read(s, pii, i);
//            }
//
//            // hulls
//            for (unsigned int i = 1; i < hullCount; ++i)
//            {
//                Serialization::Hull::Read(s, pii, i);
//            }
//
//            Serialization::Edge::ApplyInfo(pii);
//            Serialization::Patch::ApplyInfo(pii);
//            Serialization::Hull::ApplyInfo(pii);
//            Serialization::Face::ApplyInfo(pii); // has checking code, needs to be complete (last)
//
//            for (auto& hull : pii.hulls)
//            {
//                if (hull.first != nullptr)
//                {
//                    shape.AddHull(hull.first);
//                    hull.first->SetShape(&shape);
//                }
//            }
//        }
//    }
//}

//std::vector<unsigned char> Shape::Serialize()
//{
//    Serialization::PointerInfo pi(this);
//    Common::Serializer s;
//
//    Serialization::Header::Write(s);
//    Serialization::Colors::Write(s, pi);
//    Serialization::TextureCoords::Write(s, pi);
//    Serialization::Normals::Write(s, pi);
//    Serialization::Vertices::Write(s, pi);
//    Serialization::Shape::Write(s, pi, *this);
//
//    s.Flush();
//    return s.Read<std::vector<unsigned char>>();
//}
//
//bool Shape::Deserialize(const std::vector<unsigned char>& data)
//{
//    Common::Serializer s;
//    s.Write(data);
//
//    Clear();
//
//    Serialization::PointerInfoIndex pii;
//
//    try
//    {
//        Serialization::Header::Read(s);
//        Serialization::Colors::Read(s, pii.colors);
//        Serialization::TextureCoords::Read(s, pii.textureCoords);
//        Serialization::Normals::Read(s, pii.normals);
//        Serialization::Vertices::Read(s, pii.vertices);
//        Serialization::Shape::Read(s, pii, *this);
//        return true;
//    }
//    catch (const std::exception& e) 
//    {
//        Clear();
//        Common::Log::Error("Could not read Shape: {0}",e.what());
//        return false;
//    }
//}
//
//void Shape::Store(Common::DB& db)
//{
//    Serialization::PointerInfo pi(this);
//    db.BeginTransaction();
//
//    Serialization::Header::Write(db);
//    Serialization::Colors::Write(db, pi);
//    Serialization::TextureCoords::Write(db, pi);
//    Serialization::Normals::Write(db, pi);
//    Serialization::Vertices::Write(db, pi);
////    Serialization::Shape::Write(db, pi, *this);
//
//    db.EndTransaction();
//}
//
//bool Shape::Retrieve(Common::DB& db)
//{
//    Clear();
//
//    Serialization::PointerInfoIndex pii;
//
//    try
//    {
//        Serialization::Header::Read(db);
//        Serialization::Colors::Read(db, pii.colors);
//        Serialization::TextureCoords::Read(db, pii.textureCoords);
//        Serialization::Normals::Read(db, pii.normals);
//        Serialization::Vertices::Read(db, pii.vertices);
////        Serialization::Shape::Read(db, pii, *this);
//        return true;
//    }
//    catch (const std::exception& e)
//    {
//        Clear();
//        Common::Log::Error("Could not read Shape: {0}", e.what());
//        return false;
//    }
//}

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

void Shape::Store(SQLite::DB& db) const
{
    std::map<HullPtr, size_t> hulls;
    std::map<EdgePtr, size_t> edges;
    std::map<VertexPtr, size_t> vertices;
    std::map<NormalPtr, size_t> normals;
    std::map<TextureCoordPtr, size_t> textureCoords;
    std::map<FacePtr, size_t> faces;
    std::map<PatchPtr, size_t> patches;
    
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
                normals.emplace(face->GetNormal(), 0);
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


    db.ExecDML("CREATE TABLE HEADER(Key TEXT PRIMARY KEY,Value) WITHOUT ROWID");
    db.ExecDML(("INSERT INTO HEADER(Key,Value) VALUES('Version'," + std::to_string(SerializationVersion) + ")").c_str());

}

void Shape::Retrieve(SQLite::DB& db)
{
    Clear();
}
