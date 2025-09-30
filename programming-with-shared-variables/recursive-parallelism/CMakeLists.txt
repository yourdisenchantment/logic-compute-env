# ./programming-with-shared-variables/recursive-parallelism/CMakeLists.txt

cmake_minimum_required(VERSION 3.20)

find_package(Threads REQUIRED)

add_executable(pws-rp-recursive-app recursive-app.cpp)

add_executable(pws-rp-multi-thread-app multi-thread-app.cpp)
target_link_libraries(pws-rp-multi-thread-app PRIVATE Threads::Threads)

add_executable(pws-rp-multi-thread-app-ii multi-thread-app-ii.cpp)
target_link_libraries(pws-rp-multi-thread-app-ii PRIVATE Threads::Threads)

add_executable(pws-rp-restriction-thread-app restriction-thread-app.cpp)
target_link_libraries(pws-rp-restriction-thread-app PRIVATE Threads::Threads)
