#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <regex>
#include <filesystem>

#include <array>
#include <vector>

#include <cstring>

/*STL data structures*/
namespace Stl
{
    enum STL_File_Type
    {
        ascii,
        binary,
        invalid = -1,
    };
    struct Vertex
    {
        std::array<float,3> V;

        float operator[](int i) const { return V[i]; }
        float& operator[](int i) { return V[i]; }
    };
    struct Triangle
    {
        std::array<Vertex,3> vertices;
        Vertex normal;
        std::array<char,2> attribute_byte_count;
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
        return out << T.vertices[0] << '\n' << T.vertices[1] << '\n' << T.vertices[2];
    }

    int readStlFile(std::string filename, bool isBinary)
    {
        StlObject obj;

        constexpr int HEADER_SIZE = 80;
        auto fileflags = std::ios::in;
        uint32_t num_triangles;

        if (isBinary) {
            fileflags = fileflags | std::ios::binary;
            obj.filetype = binary;
        }

        std::ifstream stlfile(filename, fileflags);

        char char_header[HEADER_SIZE + 1];
        std::memset(char_header, '\0', sizeof(char_header));

        //Read the 80 byte header from the file.
        stlfile.read(char_header, HEADER_SIZE);
        obj.header = char_header;

        //Now get the total number of triangles in the object
        stlfile.read(reinterpret_cast<char*>(&num_triangles),sizeof(num_triangles));
        std::cout << num_triangles << '\n';

        obj.n_triangles = num_triangles;
        obj.tris.reserve(num_triangles);

        //Loop through the number of triangles and read the data for each of them
        float n_vec[3],vert_1[3],vert_2[3],vert_3[3];
        char attribute_byte_count[2];

        for(uint32_t i = 0; i < num_triangles; i++){
            stlfile.read(reinterpret_cast<char*>(&n_vec),sizeof(n_vec));    //Read the normal vector
            stlfile.read(reinterpret_cast<char*>(&vert_1),sizeof(vert_1));  //Read the three vertices
            stlfile.read(reinterpret_cast<char*>(&vert_2),sizeof(vert_2));
            stlfile.read(reinterpret_cast<char*>(&vert_3),sizeof(vert_3));
            stlfile.read(attribute_byte_count,sizeof(attribute_byte_count));    //Two byte attribute

            //TODO: Need constructors to change the C-style arrays to Vertex structs etc.
        }

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