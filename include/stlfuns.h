#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <functional>
#include <boost/functional/hash.hpp>

/*STL data structures*/
namespace Stl
{
    constexpr std::size_t TRIANGLE_BYTE_SIZE = 50;
    constexpr std::size_t HEADER_BYTE_SIZE = 80;

    enum STL_File_Type
    {
        ascii,
        binary,
        invalid = -1,
    };

    struct Vertex
    {
        Vertex() = default;
        Vertex(const float p0, const float p1, const float p2) : P{p0, p1, p2} {}
        bool operator==(const Vertex&) const = default;

        double x() const { return P[0]; }
        double y() const { return P[1]; }
        double z() const { return P[2]; }

        float operator[](int i) const { return P[i]; }
        float& operator[](int i) { return P[i]; }

        std::array<float,3> P;
    };

    struct Vertex_Hash
    {
        std::size_t operator()(Vertex const& V) const noexcept
        {
            std::size_t seed = 0;
            boost::hash_combine(seed,V[0]);
            boost::hash_combine(seed,V[1]);
            boost::hash_combine(seed,V[2]);
            return seed;
        }
    };

    struct Edge
    {
        Edge() = default;
        Edge(const Vertex &v1, const Vertex &v2) : p1(v1), p2(v2) {}
        
        Vertex p1;
        Vertex p2;

        friend bool operator== (const Edge &e1, const Edge &e2);
    };

    // This operator needs to be overloaded though, because it needs to account for swapped vertices
    bool operator== (const Edge &e1, const Edge &e2)
    {
        return ((e1.p1 == e2.p1) && (e1.p2 == e2.p2)) 
            || ((e1.p1 == e2.p2) && (e1.p2 == e2.p1));
    }

    struct Edge_Hash
    {
        std::size_t operator()(Edge const& E) const noexcept
        {
            std::size_t seed1 = Vertex_Hash{}(E.p1);
            boost::hash_combine(seed1, Vertex_Hash{}(E.p2));

            std::size_t seed2 = Vertex_Hash{}(E.p2);
            boost::hash_combine(seed2, Vertex_Hash{}(E.p1));

            return seed1 ^ seed2;
        }
    };

    struct Triangle
    {
        Vertex normal;
        std::array<Vertex,3> vertices;
        uint16_t attribute_byte;
    };
    struct StlObject
    {
        std::string filename;
        STL_File_Type filetype = invalid;
        std::string header = std::string(HEADER_BYTE_SIZE,'\0');
        uint32_t n_triangles = 0;
        std::vector<Triangle> tris;
    };

    [[nodiscard]] std::ostream& operator<<(std::ostream& out, Vertex& vert) noexcept 
    {
        return out << vert[0] << '\t' << vert[1] << '\t' << vert[2];
    }

    [[nodiscard]] std::ostream& operator<<(std::ostream& out, Triangle& T) noexcept 
    {
        return out << T.normal << '\n' 
            << T.vertices[0] << '\n' << T.vertices[1] << '\n' << T.vertices[2] << '\n'
            << T.attribute_byte << '\n';
    }

    [[nodiscard]] std::unordered_set<Vertex,Vertex_Hash> getVertices(const StlObject &S) noexcept
    {
        std::unordered_set<Vertex,Vertex_Hash> vertex_set;
        const std::size_t n_float_vals = S.tris.size() * 3;

        vertex_set.reserve(n_float_vals);

        for (auto &t : S.tris)
        {
            for (auto &p : t.vertices)
            {
                vertex_set.insert(p);
            }
        }

        return vertex_set;
    }

    [[nodiscard]] std::unordered_set<Edge,Edge_Hash> getEdges(const StlObject &S)
    {
        std::unordered_set<Edge,Edge_Hash> edge_set;

        for(auto &t : S.tris)
        {
            edge_set.insert(Edge(t.vertices[0],t.vertices[1]));
            edge_set.insert(Edge(t.vertices[1],t.vertices[2]));
            edge_set.insert(Edge(t.vertices[2],t.vertices[0]));
        }

        return edge_set;
    }

    [[nodiscard]] std::unordered_map<Edge,std::vector<uint32_t>,Edge_Hash> getEdgeMap(const StlObject &S)
    {
        std::unordered_map<Edge,std::vector<uint32_t>,Edge_Hash> edgemap;
        edgemap.reserve(S.n_triangles);

        for (uint32_t i = 0; i < S.n_triangles; i++)
        {
            auto tmp_tri = S.tris[i];

            edgemap[Edge(tmp_tri.vertices[0],tmp_tri.vertices[1])].push_back(i);
            edgemap[Edge(tmp_tri.vertices[1],tmp_tri.vertices[2])].push_back(i);
            edgemap[Edge(tmp_tri.vertices[2],tmp_tri.vertices[0])].push_back(i);
        }

        return edgemap;
    }

    std::size_t calculateEulerCharacteristic(const StlObject &S)
    {
        return getVertices(S).size() 
            - getEdges(S).size()
            + static_cast<std::size_t>(S.n_triangles);
    }

    void makeConnectivityList(const StlObject &S) noexcept
    {
        auto edge_map = getEdges(S);
        auto edge_vectormap = getEdgeMap(S);
        auto vertex_map = getVertices(S);

        std::cout << "Length of edge_map =\t" << edge_map.size() << '\n';
        std::cout << "Length of edge_multimap =\t" << edge_vectormap.size() << '\n';

        // For each triangle in S, find another triangle in S that shares an edge with it
        std::vector<std::array<uint32_t,3>> connectivity_map;
        connectivity_map.reserve(S.n_triangles);

        for (uint32_t i = 0; i < S.n_triangles; i++)
        {
            std::array<uint32_t,3> tmp_indices{};            

            connectivity_map.push_back(tmp_indices);
        }
    }

    void UniquifyStlObj(StlObject &S)
    {
        makeConnectivityList(S);
        return;
    }

    [[nodiscard]] StlObject readStlFileBinary(const std::string filename)
    {
        const auto fileflags = std::ios::in | std::ios::binary;
        StlObject obj;
        obj.filetype = binary;

        std::ifstream stlfile;
        obj.filename = filename;
        stlfile.open(filename, fileflags);

        if(stlfile.rdstate() != std::ios_base::goodbit)
        {
            throw std::ios_base::failure("Error opening stl file");
        }

        Triangle tmp_tri;

        //Read the 80 byte header from the file.
        stlfile.read(std::data(obj.header), HEADER_BYTE_SIZE);
        // stlfile.seekg(HEADER_BYTE_SIZE, std::ios_base::beg);

        //Now get the total number of triangles in the object
        //TODO: Get rid of all uses of sizeof() function? I'm extremely paranoid about alignment padding causing difficult-to-identify bugs.
        stlfile.read(reinterpret_cast<char*>(&obj.n_triangles),sizeof(obj.n_triangles));
        obj.tris.reserve(obj.n_triangles);

        //Loop through the number of triangles and read the data for each of them
        for(uint32_t i = 0; i < obj.n_triangles; i++){
            //Read each triangle all at once as a 50-byte chunk of data.
            //TODO: Is there a way to both read and push-back the triangle vector at the same time?
            //i.e. do this without going through an intermediate tmp_triangle variable?
            stlfile.read(reinterpret_cast<char*>(&tmp_tri), TRIANGLE_BYTE_SIZE);
            
            if (stlfile.rdstate() != std::ios_base::goodbit)
            {
                throw std::ios_base::failure("Error in stl read");
            }

            obj.tris.push_back(tmp_tri);
        }

        obj.tris.shrink_to_fit();   //This is unnecessary right now, but I don't want to forget it when I parse files that don't have an exact triangle count.
        stlfile.close();

        UniquifyStlObj(obj);
        return obj;
    }
}