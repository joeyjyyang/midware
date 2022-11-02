cmake_minimum_required(VERSION 3.16.3)

project(JOS)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(cppzmq)

add_executable(csv_parser csv_parser.cpp)
add_executable(circular_buffer circular_buffer.cpp)
#add_executable(node_handle node_handle.cpp)
add_executable(matrix matrix.cpp)
#add_executable(publisher publisher.cpp)
#add_executable(subscriber subscriber.cpp)
#add_executable(server server.cpp)
#add_executable(client client.cpp)

target_link_libraries(csv_parser)
target_link_libraries(circular_buffer)
#target_link_libraries(node_handle cppzmq)
target_link_libraries(matrix)
#target_link_libraries(publisher cppzmq)
#target_link_libraries(subscriber cppzmq)
#target_link_libraries(server cppzmq)
#target_link_libraries(client cppzmq)

# cd JOS
# mkdir build
# cd build
# cmake ..
# cmake --build .
