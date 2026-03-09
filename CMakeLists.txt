cmake_minimum_required(VERSION 3.23)
project(eep LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(main
  src/main.cpp
  src/cpu.cpp
  src/glad/src/glad.c

)

find_package(SDL3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

target_include_directories(main PRIVATE
  src/glad/include
)

target_link_libraries(main PRIVATE
  SDL3::SDL3
  OpenGL::GL
)