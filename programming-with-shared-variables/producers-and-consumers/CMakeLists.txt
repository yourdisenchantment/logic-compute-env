# ./programming-with-shared-variables/recursive-parallelism/CMakeLists.txt

cmake_minimum_required(VERSION 3.20)

find_package(Threads REQUIRED)

add_executable(pwsv-pac-shared-variables shared-variables.cpp)
target_link_libraries(pwsv-pac-shared-variables PRIVATE Threads::Threads)

add_executable(pwsv-pac-n-queens n-queens.cpp)
target_link_libraries(pwsv-pac-n-queens PRIVATE Threads::Threads)
