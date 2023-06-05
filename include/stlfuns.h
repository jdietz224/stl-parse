#pragma once
#include <array>
#include <algorithm>
#include <cmath>
#include <ranges>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <functional>
#include <gsl/narrow>

/*STL data structures*/
namespace Stl
{
    constexpr std::size_t TRIANGLE_BYTE_SIZE = 50;
    constexpr std::size_t HEADER_BYTE_SIZE = 80;

    enum STL_File_Type {
        ascii,
        binary,
        invalid = -1,
    };

    struct Vertex {
        Vertex() = default;
        ~Vertex() = default;
        Vertex(const Vertex&) = default;
        Vertex(Vertex&&) = default;
        Vertex& operator=(const Vertex&) = default;
        Vertex& operator=(Vertex&&) = default;

        Vertex(const float p0, const float p1, const float p2) : P{p0, p1, p2} {}
        bool operator==(const Vertex&) const = default;

        inline float x() const { return P[0]; }
        inline float y() const { return P[1]; }
        inline float z() const { return P[2]; }

        inline float magnitude() const { return std::sqrt((P[0]*P[0]) + (P[1]*P[1]) + (P[2]*P[2])); }

        inline float operator[](int i) const { return P[i]; }
        inline float& operator[](int i) { return P[i]; }

        std::array<float,3> P;
    };
    
    struct Triangle {
        Vertex normal;
        std::array<Vertex,3> vertices;
        uint16_t attribute_byte;
    };

    struct StlObject {
        std::string filename;
        STL_File_Type filetype = invalid;
        std::string header = std::string(HEADER_BYTE_SIZE,'\0');
        uint32_t n_triangles = 0;
        std::vector<Triangle> tris;
    };

    [[nodiscard]] auto operator+(const Vertex& v1, const Vertex& v2) noexcept -> Vertex
    {
        return Vertex(v1.x() + v2.x(), v1.y() + v2.y(), v1.z() + v2.z());
    }

    [[nodiscard]] auto operator-(const Vertex& v1, const Vertex& v2) noexcept -> Vertex
    {
        return Vertex(v1.x() - v2.x(), v1.y() - v2.y(), v1.z() - v2.z());
    }

    [[nodiscard]] auto operator*(const float& t, const Vertex& v) noexcept -> Vertex
    {
        return Vertex(v.x() * t, v.y() * t, v.z() * t);
    }

    [[nodiscard]] auto operator*(const Vertex& v, const float& t) noexcept -> Vertex
    {
        return t * v;
    }

    [[nodiscard]] auto operator/ (const Vertex& v, const float& t) noexcept -> Vertex
    {
        return v * (1/t);
    }

    [[nodiscard]] auto operator< (const Vertex& v1, const Vertex& v2) noexcept -> bool
    {
        if (v1.x() == v2.x()) {
            if (v1.y() == v2.y()) {
                return v1.z() < v2.z();
            }
            return v1.y() < v2.y();
        }
        return v1.x() < v2.x();
    }

    [[nodiscard]] auto operator<<(std::ostream& out, Vertex& vert) noexcept -> std::ostream& {
        return out << vert.x() << '\t' << vert.y() << '\t' << vert.z();
    }

    [[nodiscard]] auto operator<<(std::ostream& out, Triangle& T) noexcept -> std::ostream& {
        return out << T.normal << '\n' 
            << T.vertices[0] << '\n' << T.vertices[1] << '\n' << T.vertices[2] << '\n'
            << T.attribute_byte << '\n';
    }

    [[nodiscard]] auto normal_vector(const Vertex p0, const Vertex p1, const Vertex p2) noexcept -> Vertex
    {
        Vertex v = p1 - p0;
        Vertex w = p2 - p0;

        Vertex nhat = Vertex(   (v.y() * w.z()) - (v.z() * w.y()),
                                (v.z() * w.x()) - (v.x() * w.z()),
                                (v.x() * w.y()) - (w.x() * v.y()));


        return nhat/nhat.magnitude();
    }

    [[nodiscard]] auto normal_vector(const Triangle& t) noexcept -> Vertex
    {
        return normal_vector(t.vertices[0], t.vertices[1], t.vertices[2]);
    }

    [[nodiscard]] auto getVertexBuffer(const StlObject &S) noexcept -> std::vector<Vertex> {
        std::vector<Vertex> vertex_vector;
        vertex_vector.reserve(S.n_triangles);

        for (auto &t : S.tris)
        {
            vertex_vector.push_back(t.vertices[0]);
            vertex_vector.push_back(t.vertices[1]);
            vertex_vector.push_back(t.vertices[2]);
        }

        std::sort(vertex_vector.begin(), vertex_vector.end());
        auto endItr = std::unique(vertex_vector.begin(), vertex_vector.end());

        vertex_vector.shrink_to_fit();

        return vertex_vector;
    }

    [[nodiscard]] auto processStlObject(const StlObject &S) noexcept -> std::pair<std::vector<Vertex>,std::vector<std::array<uint32_t,3>>> {
        const auto vertex_buffer = getVertexBuffer(S);

        std::vector<std::array<uint32_t,3>> index_buffer;
        index_buffer.reserve(vertex_buffer.size());

        const auto i_beg = vertex_buffer.begin();
        const auto i_end = vertex_buffer.end();

        for (auto &t : S.tris) {
            auto iter_0 = std::lower_bound(i_beg, i_end, t.vertices[0]);
            auto iter_1 = std::lower_bound(i_beg, i_end, t.vertices[1]);
            auto iter_2 = std::lower_bound(i_beg, i_end, t.vertices[2]);

            auto i0 = gsl::narrow<uint32_t>(std::distance(i_beg, iter_0));
            auto i1 = gsl::narrow<uint32_t>(std::distance(i_beg, iter_1));
            auto i2 = gsl::narrow<uint32_t>(std::distance(i_beg, iter_2));

            index_buffer.push_back(std::array<uint32_t,3>{i0,i1,i2});
        }

        return make_pair(vertex_buffer,index_buffer);
    }

    auto computeStlNormals(StlObject& S) -> void
    {
        for (auto& tri : S.tris) {
            tri.normal = normal_vector(tri);
        }
    }

    [[nodiscard]] auto readStlFileBinary(const std::string filename, bool normals_provided = false) -> StlObject {
        const auto fileflags = std::ios::in | std::ios::binary;
        StlObject obj;
        obj.filetype = binary;

        std::ifstream stlfile;
        obj.filename = filename;
        stlfile.open(filename, fileflags);

        if(stlfile.rdstate() != std::ios_base::goodbit) {
            throw std::ios_base::failure("Error opening stl file");
        }

        Triangle tmp_tri{};

        //Read the 80 byte header from the file.
        stlfile.read(std::data(obj.header), HEADER_BYTE_SIZE);

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
            
            if (stlfile.rdstate() != std::ios_base::goodbit) {
                throw std::ios_base::failure("Error in stl read");
            }

            obj.tris.push_back(tmp_tri);
        }

        obj.tris.shrink_to_fit();   //This is unnecessary right now, but I don't want to forget it when I parse files that don't have an exact triangle count.
        stlfile.close();

        if (!normals_provided) {
            computeStlNormals(obj);
        }
        
        return obj;
    }
}