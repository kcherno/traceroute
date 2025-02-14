cmake_minimum_required(VERSION 3.27.0)

project(TRACEROUTE LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD          20)

add_executable(traceroute
    ${CMAKE_SOURCE_DIR}/src/configuration/program_options.cpp
    ${CMAKE_SOURCE_DIR}/src/traceroute.cpp)

target_include_directories(traceroute
    PUBLIC ${CMAKE_SOURCE_DIR}/proto/include
    PUBLIC ${CMAKE_SOURCE_DIR}/include)

find_package(Boost REQUIRED program_options)

target_link_libraries(traceroute
    PUBLIC ${CMAKE_SOURCE_DIR}/proto/lib/libproto.a
    PUBLIC Boost::program_options)

set_target_properties(traceroute PROPERTIES RUNTIME_OUTPUT_DIRECTORY bin)
