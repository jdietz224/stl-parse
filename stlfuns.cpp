#include <iostream>
#include <fstream>

#include "stlfuns.h"

int main()
{
    std::fstream stl_file;

    stl_file.open("test-files/sphere_ascii.stl");
    std::string sa;
    while(std::getline(stl_file,sa)){
        std::cout << sa << '\n';
    }
}