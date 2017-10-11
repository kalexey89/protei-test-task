cmake_minimum_required(VERSION 3.5)

project(protei CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED on)

add_subdirectory(network)
add_subdirectory(server)
