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
    # from DrawKit library
    Libraries/DrawKit/src/Outline.cpp
    Libraries/DrawKit/src/Fill.cpp
    Libraries/DrawKit/src/Widget.cpp
    # from HtmlKit library
    Libraries/HtmlKit/src/Tokenize.cpp
    Libraries/HtmlKit/src/Parse.cpp
    # from RenderKit library
    Libraries/RenderKit/src/Typeface.cpp
    Libraries/RenderKit/src/Text.cpp
    Libraries/RenderKit/src/Layout.cpp
    Libraries/RenderKit/src/Paint.cpp
    # from NetKit library
    Libraries/NetKit/src/Url.cpp
    Libraries/NetKit/src/Fetch.cpp
    Libraries/NetKit/src/Http.cpp
)
if(WIN32)
      target_sources(tesseract PRIVATE Libraries/NetKit/src/SocketWin.cpp)
      target_sources(tesseract PRIVATE Src/WinPrint.cpp)
      target_link_libraries(tesseract PRIVATE ws2_32)
else()
      target_sources(tesseract PRIVATE Libraries/NetKit/src/SocketUnix.cpp)
endif()

target_include_directories(tesseract PRIVATE
    Libraries
)

target_link_libraries(tesseract PRIVATE
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
)

# GUI app: no console window on Windows (stderr goes nowhere in release).
# Console tools in Tools/ keep theirs.
if(WIN32)
      set_target_properties(tesseract PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

add_subdirectory(Tools)
