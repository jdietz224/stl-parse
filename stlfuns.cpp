#include <iostream>
#include <fstream>

#include "stlfuns.h"

int main()
{
    std::fstream stl_file;

    const std::string ascii_filename = "test-files/sphere_ascii.stl";

    stl_file.open(ascii_filename);
    std::string sa;
    while(std::getline(stl_file,sa)){
        std::cout << sa << '\n';
    }
}