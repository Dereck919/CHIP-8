cmake_minimum_required(VERSION 3.23)

project(eep LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(main
    src/main.cpp
    src/cpu.cpp
)

find_package(SDL3 CONFIG REQUIRED)

target_link_libraries(main PRIVATE SDL3::SDL3)