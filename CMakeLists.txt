cmake_minimum_required(VERSION 3.16)

project(myers LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -Wall -Wextra")

enable_testing()


add_subdirectory(src)
add_subdirectory(tests)

add_executable(myers_diff main.cpp)
target_link_libraries(myers_diff
	PRIVATE
	 myers)
