#include <iostream>
#include <fstream>
#include <cstdio>

#include "stlfuns.h"

int main()
{
    std::ifstream stl_file_ascii;
    //std::basic_ifstream<uint32_t> stl_file_binary;
    std::ifstream stl_file_binary;

    const std::string stl_ascii_filename = "test-files/sphere_ascii.stl";
    const std::string stl_binary_filename = "test-files/femur_binary.stl";
    std::string ln;
    uint32_t n_tris = 0;



    std::vector<Stl::Triangle> tri_vec;


    stl_file_ascii.open(stl_ascii_filename);

    while(std::getline(stl_file_ascii,ln)){
        //std::cout << ln << std::endl;
        //std::cout << stl_file.tellg() << std::endl;
    }

    stl_file_ascii.close();


    char c[80], att_byte_count[2];
    stl_file_binary.open(stl_binary_filename,std::ios::binary);
    stl_file_binary.read(c, sizeof c);
    stl_file_binary.read(reinterpret_cast<char*>(&n_tris), sizeof n_tris);

    std::printf("%s",c);
    std::cout << '\n' << n_tris << std::endl;

    tri_vec.reserve(n_tris);
    Stl::Triangle tmp_tri;
    Stl::Vertex tmp_v1,tmp_v2,tmp_v3,tmp_normal;

    std::cout << "Reading triangles" << std::endl;
    for(int i=0; i<n_tris; i++){
        stl_file_binary.read(reinterpret_cast<char*>(&tmp_normal), sizeof tmp_normal);
        stl_file_binary.read(reinterpret_cast<char*>(&tmp_v1), sizeof tmp_v1);
        stl_file_binary.read(reinterpret_cast<char*>(&tmp_v2), sizeof tmp_v2);
        stl_file_binary.read(reinterpret_cast<char*>(&tmp_v3), sizeof tmp_v3);
        stl_file_binary.read(att_byte_count, sizeof att_byte_count);

        tmp_tri.vertices[0] = tmp_v1;
        tmp_tri.vertices[1] = tmp_v2;
        tmp_tri.vertices[2] = tmp_v3;
        tmp_tri.normal = tmp_normal;

        tri_vec.push_back(tmp_tri);
    }
    std::cout << "Finished reading triangles" << std::endl;

    tmp_v1 = tri_vec[0].vertices[0];
    auto tmp_n = tri_vec[0].normal;
    //tmp_v2 = tri_vec[1].vertices[1];
    //tmp_v3 = tri_vec[2].vertices[2];

    std::cout << tmp_v1.V[0] << '\t' << tmp_v1.V[1] << '\t' << tmp_v1.V[2] << '\n'
        << tmp_n.V[0] << '\t' << tmp_n.V[1] << '\t' << tmp_n.V[2] << std::endl;

    stl_file_binary.close();
}