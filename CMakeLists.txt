cmake_minimum_required(VERSION 2.8)

project(genmap)

set(SOURCE_EXE main.cpp codegen.cpp mapgen.cpp)

#include_directories(foo)
set (CMAKE_CXX_COMPILER g++-5)
set (CMAKE_CXX_FLAGS --std=c++11)

add_executable(genmap ${SOURCE_EXE})

#add_subdirectory(foo)

#add_library(foo STATIC IMPORTED)
##set_property(TARGET foo PROPERTY
#             IMPORTED_LOCATION foo/libfoo.a)

#target_link_libraries(main foo)
