project(Graph CXX)
set(CMAKE_CXX_STANDARD 17)
cmake_minimum_required(VERSION 3.1)

include(CTest)

if(EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    set(HAS_CONAN ON)
else()
    set(HAS_CONAN OFF)
endif()
set(USE_CONAN "${HAS_CONAN}" CACHE BOOL
    "Use conan to resolve dependencies.")

set(TEST_COVERAGE OFF CACHE BOOL
    "Report code test coverage.")

if(USE_CONAN)
    # If we're using conan, let it handle dependencies.
    include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    conan_basic_setup()
    if(BUILD_TESTING)
        list(APPEND CMAKE_MODULE_PATH "${CONAN_LIB_DIRS_CATCH2}/cmake/Catch2")
    endif()
else()
    # If we don't have conan, use the submodules.
    include_directories(extern/range-v3/include)
    if(BUILD_TESTING)
        include_directories(extern/Catch2/single_include)
        list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/extern/Catch2/contrib")
    endif()
endif()

include_directories("include")

if(BUILD_TESTING)
    file(GLOB TEST_SOURCES test/*.cpp)
    add_executable(all_tests ${TEST_SOURCES})

    if(TEST_COVERAGE)
        target_compile_options(all_tests PRIVATE -g -fprofile-arcs -ftest-coverage --coverage -O0 -fno-inline -fno-inline-small-functions -fno-default-inline)
        # target_link_options would be better but requires a recent CMake version.
        target_link_libraries(all_tests PRIVATE --coverage)
    endif()

    include(Catch)
    catch_discover_tests(all_tests)
endif()
