# ./programming-with-shared-variables/iterative-parallelism/CMakeLists.txt

cmake_minimum_required(VERSION 3.20)

find_package(Threads REQUIRED)

add_executable(pws-single-thread-app single-thread-app.cpp)

add_executable(pws-multi-thread-app multi-thread-app.cpp)
target_link_libraries(pws-multi-thread-app PRIVATE Threads::Threads)

add_executable(pws-k-thread-app k-thread-app.cpp)
target_link_libraries(pws-k-thread-app PRIVATE Threads::Threads)
