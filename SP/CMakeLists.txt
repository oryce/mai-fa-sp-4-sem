cmake_minimum_required(VERSION 3.27 FATAL_ERROR)

project(sp_lab_1 LANGUAGES C)

set(CMAKE_C_FLAGS "-Wall -pedantic -std=c17")

if (CMAKE_BUILD_TYPE STREQUAL "ASAN")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
endif ()

# Enable POSIX features
add_compile_definitions(
    _DEFAULT_SOURCE
)

add_subdirectory(task1)
add_subdirectory(task2)
add_subdirectory(task3)
add_subdirectory(task4)
add_subdirectory(task5)
add_subdirectory(task6)
add_subdirectory(task7)