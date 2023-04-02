#include <algorithm>
#include <string>
#include <iostream>
#include <limits>

#include "stlfuns.h"

[[nodiscard]] auto getVertexExtents(const std::vector<Stl::Vertex> &vertex_buffer) noexcept -> std::array<std::pair<float,float>,3> {
    constexpr auto hbound = std::numeric_limits<float>::max(), lbound = std::numeric_limits<float>::min();

    float xmin = hbound, ymin = hbound, zmin = hbound, xmax = lbound, ymax = lbound, zmax = lbound;

    for (auto &v : vertex_buffer) {
        xmin = std::min(xmin, v[0]);
        xmax = std::max(xmax, v[0]);

        ymin = std::min(ymin, v[1]);
        ymax = std::max(ymax, v[1]);
        
        zmin = std::min(zmin, v[2]);
        zmax = std::max(zmax, v[2]);
    }

    auto xbounds = std::make_pair(xmin, xmax);
    auto ybounds = std::make_pair(ymin, ymax);
    auto zbounds = std::make_pair(zmin, zmax);

    return std::array<std::pair<float,float>,3>({xbounds, ybounds, zbounds});
}

int main()
{
    const std::string stl_ascii_filename = "../test-files/sphere_ascii.stl";
    const std::string stl_binary_filename = "../test-files/femur_binary.stl";

    auto S = Stl::readStlFileBinary(stl_binary_filename);
    std::cout << "# of triangles: " << S.n_triangles << '\n';

    auto [vertex_buffer, index_buffer] = processStlObject(S);
    
    auto bounds = getVertexExtents(vertex_buffer);

    for (auto &m : bounds){
        std::cout << m.first << '\t' << m.second << '\n';
    }

    return 0;
}