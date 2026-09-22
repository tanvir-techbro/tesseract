##
## Tesseract - A simple browser engine.
## https://github.com/tanvir-techbro/tesseract
##
## Copyright (c) 2026-present tanvir-techbro (Tanvir)
## Released under the MIT License.
##

# Developer tools (lexdump, treedump, ...). Built with the main project.

add_executable(lexdump
    Debug/LexDump.cpp
    ../Libreries/HtmlKit/src/Tokenize.cpp
)

set_target_properties(lexdump PROPERTIES OUTPUT_NAME LexDump)

target_include_directories(lexdump PRIVATE
    ../Libreries
)

add_executable(treedump
    Debug/TreeDump.cpp
    ../Libreries/HtmlKit/src/Tokenize.cpp
    ../Libreries/HtmlKit/src/Parse.cpp
)

set_target_properties(treedump PROPERTIES OUTPUT_NAME TreeDump)

target_include_directories(treedump PRIVATE
    ../Libreries
)
