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

option(ENABLE_ASAN "Enable AddressSanitizer" OFF)

if(ENABLE_ASAN)
      add_compile_options(-fsanitize=address -fno-omit-frame-pointer -g)
      add_link_options(-fsanitize=address)
endif()

add_executable(tesseract
    Src/Tess.cpp
    # from DrawKit librery
    Libreries/DrawKit/src/Outline.cpp
    Libreries/DrawKit/src/Fill.cpp
    # from HtmlKit librery
    Libreries/HtmlKit/src/Tokenize.cpp
    Libreries/HtmlKit/src/Parse.cpp
    # from RenderKit librery
    Libreries/RenderKit/src/Render.cpp
)

target_include_directories(tesseract PRIVATE
    Libreries
)

target_link_libraries(tesseract PRIVATE
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
)

add_subdirectory(Tools)
