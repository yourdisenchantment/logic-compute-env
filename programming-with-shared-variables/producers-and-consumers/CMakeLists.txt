# programming-with-shared-variables/producers-and-consumers/CMakeLists.txt

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

add_executable(pwsv-pac-n-queens-producer n-queens-producer.cpp)

add_executable(pwsv-pac-n-queens-consumer n-queens-consumer.cpp)
target_include_directories(pwsv-pac-n-queens-consumer PRIVATE ${PROJECT_INCLUDE_DIR})
target_compile_definitions(pwsv-pac-n-queens-consumer PRIVATE
        PRODUCER_PATH="$<TARGET_FILE:pwsv-pac-n-queens-producer>"
)
add_dependencies(pwsv-pac-n-queens-consumer pwsv-pac-n-queens-producer)
