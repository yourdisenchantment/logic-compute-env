# ./programming-with-shared-variables/distributed-computing/CMakeLists.txt

cmake_minimum_required(VERSION 4.0)
find_package(MPI REQUIRED)

add_executable(pwsv-dc-divisible-app divisible-app.cpp)
target_compile_features(pwsv-dc-divisible-app PRIVATE cxx_std_20)
target_link_libraries(pwsv-dc-divisible-app PRIVATE MPI::MPI_CXX)

add_executable(pwsv-dc-multiply-app multiply-app.cpp)
target_compile_features(pwsv-dc-multiply-app PRIVATE cxx_std_20)
target_link_libraries(pwsv-dc-multiply-app PRIVATE MPI::MPI_CXX)
