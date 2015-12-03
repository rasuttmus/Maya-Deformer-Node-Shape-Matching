cmake_minimum_required(VERSION 2.8)
project(sampleplugin)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIRE}/install)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

add_subdirectory(src)