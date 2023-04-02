cmake_minimum_required(VERSION 3.10)
project(stl LANGUAGES CXX)
add_executable(stlfuns stlfuns.cpp)
set_property(TARGET stlfuns PROPERTY CXX_STANDARD 20)
target_include_directories(stlfuns PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
#add_subdirectory(/home/jdietz/opt/GSL ./gsl)   # This doesn't work and at some point I should figure out why.
target_include_directories(stlfuns PRIVATE /home/jdietz/opt/GSL/include)
target_compile_options(stlfuns PRIVATE "-O0" "-ggdb3")
