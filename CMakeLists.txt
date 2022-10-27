cmake_minimum_required(VERSION 3.16.3)

project(JOS)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(cppzmq)

add_executable(circular_buffer circular_buffer.cpp)
add_executable(server server.cpp)
add_executable(client client.cpp)

target_link_libraries(circular_buffer cppzmq)
target_link_libraries(server cppzmq)
target_link_libraries(client cppzmq)

# cd JOS
# mkdir build
# cd build
# cmake ..
# cmake --build .
