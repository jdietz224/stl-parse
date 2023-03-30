cmake_minimum_required(VERSION 3.10)
project(stl LANGUAGES CXX)
add_executable(stlfuns stlfuns.cpp)
target_include_directories(stlfuns PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
