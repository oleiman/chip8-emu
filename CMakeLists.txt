cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -Wno-implicit-fallthrough -pedantic")

project(chip8-emu)
add_executable(chip8-emu main.cpp chip8.cpp)
set_property(TARGET chip8-emu PROPERTY CXX_STANDARD 14)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} ./include)

target_link_libraries(chip8-emu ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )
