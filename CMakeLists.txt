cmake_minimum_required(VERSION 3.19)
project(ACS37800 C)

set(CMAKE_C_STANDARD 99)

add_library(ACS37800 ACS37800.c ACS37800.h)

# add dependencies
include(cpm/CPM.cmake)
CPMAddPackage("gh:JeroenVandezande/LowLevelCInterfaces@1.0.6")

target_include_directories (ACS37800 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(ACS37800 PUBLIC LowLevelCInterfaces m)