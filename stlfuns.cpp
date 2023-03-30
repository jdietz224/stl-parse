#include <string>
#include <iostream>

#include "stlfuns.h"

int main()
{
    const std::string stl_ascii_filename = "../test-files/sphere_ascii.stl";
    const std::string stl_binary_filename = "../test-files/femur_binary.stl";

    auto S = Stl::readStlFileBinary(stl_binary_filename);
    std::cout << "# of triangles: " << S.n_triangles << '\n';
    return 0;
}