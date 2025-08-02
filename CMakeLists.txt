cmake_minimum_required(VERSION 3.20)
project(frac LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL3 REQUIRED)

add_executable(frac src/main.cc)

target_link_libraries(frac PRIVATE SDL3::SDL3)

install(TARGETS frac DESTINATION bin)
