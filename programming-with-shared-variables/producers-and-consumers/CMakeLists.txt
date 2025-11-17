# ./programming-with-shared-variables/producers-and-consumers/CMakeLists.txt

cmake_minimum_required(VERSION 4.0)

find_package(Threads REQUIRED)

set(PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(pwsv-pac-shared-variables shared-variables.cpp)
target_link_libraries(pwsv-pac-shared-variables PRIVATE Threads::Threads)

add_executable(pwsv-pac-n-queens n-queens.cpp)
target_include_directories(pwsv-pac-n-queens PRIVATE ${PROJECT_INCLUDE_DIR})
target_link_libraries(pwsv-pac-n-queens PRIVATE Threads::Threads)

add_executable(pwsv-pac-n-queens-shared n-queens-shared.cpp)
target_include_directories(pwsv-pac-n-queens-shared PRIVATE ${PROJECT_INCLUDE_DIR})
target_link_libraries(pwsv-pac-n-queens-shared PRIVATE Threads::Threads)

add_executable(pwsv-pac-n-queens-popen n-queens-popen.cpp)
target_include_directories(pwsv-pac-n-queens-popen PRIVATE ${PROJECT_INCLUDE_DIR})
target_link_libraries(pwsv-pac-n-queens-popen PRIVATE Threads::Threads)
