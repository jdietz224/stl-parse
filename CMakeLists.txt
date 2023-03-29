add_library(stlfuns ${CMAKE_CURRENT_SOURCE_DIR}/include/stlfuns.h)
set_target_properties(stlfuns PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(stlfuns INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
