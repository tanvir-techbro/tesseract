##
## Tesseract - A simple browser engine.
## https://github.com/tanvir-techbro/tesseract
##
## Copyright (c) 2026-present tanvir-techbro (Tanvir)
## Released under the MIT License.
##

cmake_minimum_required(VERSION 3.20)

project(tesseract VERSION 1.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL3 REQUIRED)
find_package(SDL3_ttf REQUIRED)

add_executable(tesseract
    Src/Tess.cpp
    # from DrawKit librery
    Libreries/DrawKit/src/Outline.cpp
    Libreries/DrawKit/src/Fill.cpp
    # from HtmlKit librery
    Libreries/HtmlKit/src/Tokenize.cpp
    Libreries/HtmlKit/src/Parse.cpp
)

target_include_directories(tesseract PRIVATE
    Libreries
)

target_link_libraries(tesseract PRIVATE
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
)
