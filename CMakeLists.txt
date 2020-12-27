cmake_minimum_required(VERSION 3.5)

project(dashboard)

find_package(Threads REQUIRED)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3")

include_directories(libmyaio/inc)

add_subdirectory(libmyaio)
add_subdirectory(app)
add_subdirectory(dashboard)
