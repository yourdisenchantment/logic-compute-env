# CMakeLists.txt

cmake_minimum_required(VERSION 4.0)
project(logic_compute_env LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Wpedantic)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(programming-with-shared-variables/iterative-parallelism)
add_subdirectory(programming-with-shared-variables/recursive-parallelism)
add_subdirectory(programming-with-shared-variables/producers-and-consumers)
add_subdirectory(programming-with-shared-variables/test-work)
