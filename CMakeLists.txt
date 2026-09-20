##
## Tesseract - A simple 2D browserlike rendering engine.
## https://github.com/tanvir-techbro/tesseract
##
## Copyright (c) 2026-present tanvir-techbro (Tanvir)
## Released under the MIT License.
##

cmake_minimum_required(VERSION 3.20)

project(tesseract VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(tesseract
    src/main.cpp
)

find_package(SDL3 REQUIRED)
target_link_libraries(tesseract PRIVATE SDL3::SDL3)
