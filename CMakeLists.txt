cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(na-engine LANGUAGES C VERSION 0.1)

set(CMAKE_C_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES})

add_compile_options(-Wall -Wextra -ansi -pedantic -Wunused)
add_executable(driver_c  driver.c  fuse.c)
# add_executable(driver_cpp driver.cpp  fuse.c)
