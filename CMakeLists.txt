cmake_minimum_required(VERSION 3.16.3)

project(JOS)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#find_package(cppzmq)

add_executable(smart_pointers smart_pointers.cpp)
add_executable(csv_parser csv_parser.cpp)
add_executable(circular_buffer circular_buffer.cpp)
add_executable(matrix matrix.cpp)
add_executable(driver driver.cpp)
add_executable(scheduler scheduler.cpp)
add_executable(metrics metrics.cpp)
add_executable(rule_of_5 rule_of_5.cpp)
#add_executable(publisher_node publisher_node.cpp)
#add_executable(subscriber_node subscriber_node.cpp)
#add_executable(server_node server_node.cpp)
#add_executable(client_node client_node.cpp)

target_link_libraries(smart_pointers)
target_link_libraries(csv_parser)
target_link_libraries(circular_buffer)
target_link_libraries(matrix)
target_link_libraries(driver)
target_link_libraries(scheduler)
target_link_libraries(metrics)
target_link_libraries(rule_of_5)
#target_link_libraries(publisher_node cppzmq)
#target_link_libraries(subscriber_node cppzmq)
#target_link_libraries(server_node cppzmq)
#target_link_libraries(client_node cppzmq)

# cd midware
# mkdir build
# cd build
# cmake ..
# cmake --build .
