project(Graph CXX)
set (CMAKE_CXX_STANDARD 17)
cmake_minimum_required(VERSION 3.1)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include_directories("include")

add_executable(all_tests test/main.cpp test/Adjacency_list.cpp test/Atomic_adjacency_list.cpp test/Dot_format.cpp test/Edge_list.cpp test/Stable_adjacency_list.cpp test/Stable_edge_list.cpp test/Stable_node_list.cpp)
target_link_libraries(all_tests ${CONAN_LIBS})
