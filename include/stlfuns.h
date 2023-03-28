#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

/*STL data structures*/
namespace Stl
{
    typedef std::array<float,3> Vertex;
    constexpr std::size_t TRIANGLE_SIZE = 50;
    constexpr std::size_t HEADER_SIZE = 80;

    enum STL_File_Type
    {
        ascii,
        binary,
        invalid = -1,
    };
    struct Triangle
    {
        Vertex normal;
        std::array<Vertex,3> vertices;
        uint16_t attribute_byte;
    };
    struct StlObject
    {
        STL_File_Type filetype = invalid;
        std::string header = std::string(HEADER_SIZE,'\0');
        uint32_t n_triangles = 0;
        std::vector<Triangle> tris;
    };

    std::ostream& operator<<(std::ostream& out, Vertex& vert)
    {
        return out << vert[0] << '\t' << vert[1] << '\t' << vert[2];
    }

    std::ostream& operator<<(std::ostream& out, Triangle& T)
    {
        return out << T.normal << '\n' 
            << T.vertices[0] << '\n' << T.vertices[1] << '\n' << T.vertices[2] << '\n' 
            << T.attribute_byte << '\n';
    }

    StlObject readStlFile(std::string filename, bool isBinary = true)
    {
        auto fileflags = std::ios::in;
        StlObject obj;
        obj.header.reserve(HEADER_SIZE);

        if (isBinary) {
            fileflags = fileflags | std::ios::binary;
            obj.filetype = binary;
        }

        std::ifstream stlfile;
        stlfile.open(filename, fileflags);
        Triangle tmp_tri;

        //Read the 80 byte header from the file.
        stlfile.read(std::data(obj.header), HEADER_SIZE);

        //Now get the total number of triangles in the object
        //TODO: Get rid of all uses of sizeof() function? I'm extremely paranoid about alignment padding causing difficult-to-identify bugs.
        stlfile.read(reinterpret_cast<char*>(&obj.n_triangles),sizeof(obj.n_triangles));
        obj.tris.reserve(obj.n_triangles);

        //Loop through the number of triangles and read the data for each of them
        for(uint32_t i = 0; i < obj.n_triangles; i++){
            //Read each triangle all at once as a 50-byte chunk of data.
            //TODO: Is there a way to both read and push-back the triangle vector at the same time?
            //i.e. do this without going through an intermediate tmp_triangle variable?
            stlfile.read(reinterpret_cast<char*>(&tmp_tri), TRIANGLE_SIZE);
            obj.tris.push_back(tmp_tri);
        }

        stlfile.close();

        return obj;
    }
}