cmake_minimum_required(VERSION 3.16.3)

project(JOS)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(cppzmq)

add_executable(test test.cpp)
add_executable(server server.cpp)
add_executable(client client.cpp)

target_link_libraries(test cppzmq)
target_link_libraries(server cppzmq)
target_link_libraries(client cppzmq)

# cd JOS
# mkdir build
# cd build
# cmake ..
# cmake --build .
