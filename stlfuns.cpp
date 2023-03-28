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

    auto S = Stl::readStlFile(stl_binary_filename, true);
    
    return 0;
}