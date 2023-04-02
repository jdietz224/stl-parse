#include <array>
#include <algorithm>
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

        friend bool operator< (const Vertex &v1, const Vertex &v2);
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

    bool operator< (const Vertex &v1, const Vertex &v2)
    {
        if (v1.P[0] == v2.P[0])
        {
            if (v1.P[1] == v2.P[1])
            {
                return v1.P[2] < v2.P[2];
            }

            return v1.P[1] < v2.P[1];
        }

        return v1.P[0] < v2.P[0];
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

    [[nodiscard]] std::vector<Vertex> processSTL(const StlObject &S) noexcept
    {

        std::vector<Vertex> vertex_vector;
        vertex_vector.reserve(S.n_triangles);

        for (auto &t : S.tris)
        {
            vertex_vector.push_back(t.vertices[0]);
            vertex_vector.push_back(t.vertices[1]);
            vertex_vector.push_back(t.vertices[2]);
        }

        std::sort(vertex_vector.begin(), vertex_vector.end());
        std::unique(vertex_vector.begin(), vertex_vector.end());

        vertex_vector.shrink_to_fit();

        return vertex_vector;
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

        auto v = processSTL(obj);

        return obj;
    }
}