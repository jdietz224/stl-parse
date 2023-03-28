#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <regex>
#include <filesystem>

#include <array>
#include <vector>
#include <algorithm>

#include <cstring>
#include <chrono>

/*STL data structures*/
namespace Stl
{
    typedef std::array<float,3> Vertex;
    constexpr std::size_t TRIANGLE_SIZE = 50;

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
        std::string header;
        uint32_t n_triangles = 0;
        std::vector<Triangle> tris;
        STL_File_Type filetype;
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

    int readStlFile(std::string filename, bool isBinary)
    {
        constexpr int HEADER_SIZE = 80;
        auto fileflags = std::ios::in;
        uint32_t num_triangles;

        StlObject obj;
        obj.header.reserve(HEADER_SIZE);

        if (isBinary) {
            fileflags = fileflags | std::ios::binary;
            obj.filetype = binary;
        } else {
            obj.filetype = ascii;
        }

        std::ifstream stlfile(filename, fileflags);
        std::string tmp_header(HEADER_SIZE, '\0');

        //Read the 80 byte header from the file.
        stlfile.read(std::data(tmp_header), HEADER_SIZE);
        std::cout << "File Header: " << tmp_header << '\n';

        //Now get the total number of triangles in the object
        stlfile.read(reinterpret_cast<char*>(&num_triangles),sizeof(num_triangles));
        std::cout << "# triangles: " << num_triangles << '\n';

        obj.n_triangles = num_triangles;
        obj.tris.reserve(num_triangles);

        //Loop through the number of triangles and read the data for each of them
        Vertex n,v1,v2,v3;
        uint16_t attribute_byte;
        Triangle tmp_tri;

        auto begin = std::chrono::high_resolution_clock::now();
        for(uint32_t i = 0; i < num_triangles; i++){
            //Read each triangle all at once as a 50-byte chunk of data.
            stlfile.read(reinterpret_cast<char*>(&tmp_tri), TRIANGLE_SIZE);
            obj.tris.push_back(tmp_tri);
        }
        auto end = std::chrono::high_resolution_clock::now();

        auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end-begin);
        std::cout << "File processing time: " << diff.count() << " microseconds\n";

        stlfile.close();

        return 0;
    }
}

/* Regex Functions*/

void test_regex(){
    std::string s = "Some people, when confronted with a problem, think "
        "\"I know, I'll use regular expressions.\" "
        "Now they have two problems.";
 
    std::regex self_regex("REGULAR EXPRESSIONS",
            std::regex_constants::ECMAScript | std::regex_constants::icase);
    if (std::regex_search(s, self_regex)) {
        std::cout << "Text contains the phrase 'regular expressions'\n";
    }
 
    std::regex word_regex("(\\w+)");
    auto words_begin = 
        std::sregex_iterator(s.begin(), s.end(), word_regex);
    auto words_end = std::sregex_iterator();
 
    std::cout << "Found "
              << std::distance(words_begin, words_end)
              << " words\n";
 
    const int N = 6;
    std::cout << "Words longer than " << N << " characters:\n";
    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        std::smatch match = *i;
        std::string match_str = match.str();
        if (match_str.size() > N) {
            std::cout << "  " << match_str << '\n';
        }
    }
 
    std::regex long_word_regex("(\\w{7,})");
    std::string new_s = std::regex_replace(s, long_word_regex, "[$&]");
    std::cout << new_s << '\n';
}