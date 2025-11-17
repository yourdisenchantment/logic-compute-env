# ./programming-with-shared-variables/iterative-parallelism/CMakeLists.txt

cmake_minimum_required(VERSION 4.0)

find_package(Threads REQUIRED)

add_executable(pwsv-ip-single-thread-app single-thread-app.cpp)

add_executable(pwsv-ip-multi-thread-app multi-thread-app.cpp)
target_link_libraries(pwsv-ip-multi-thread-app PRIVATE Threads::Threads)

add_executable(pwsv-ip-k-thread-app k-thread-app.cpp)
target_link_libraries(pwsv-ip-k-thread-app PRIVATE Threads::Threads)
