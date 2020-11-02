cmake_minimum_required(VERSION 3.5)

project(UCG-GFRD-Driver
        LANGUAGES C CXX)

add_subdirectory(UCG-GFRD-Driver)
enable_testing()
add_subdirectory(test)
