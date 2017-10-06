cmake_minimum_required(VERSION 2.8)

project(glfw3_prj)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++11")
include_directories("include")
add_executable(${PROJECT_NAME} "main.cpp" "glad.c" "uilt.cpp")
target_link_libraries(${PROJECT_NAME} assimp glfw GL X11 Xrandr Xinerama Xxf86vm Xcursor dl)
add_executable(s "test.cpp" "uilt.cpp" "glad.c")
target_link_libraries(s assimp glfw GL X11 Xrandr Xinerama Xxf86vm Xcursor dl)
