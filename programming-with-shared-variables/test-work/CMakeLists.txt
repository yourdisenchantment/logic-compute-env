# programming-with-shared-variables/test-work/CMakeLists.txt

cmake_minimum_required(VERSION 4.0)

find_package(Threads REQUIRED)

add_executable(pwsv-tw-min-array-elem min-array-elem.cpp)
target_link_libraries(pwsv-tw-min-array-elem PRIVATE Threads::Threads)
