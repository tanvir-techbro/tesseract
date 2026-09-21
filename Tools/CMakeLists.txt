##
## Tesseract - A simple browser engine.
## https://github.com/tanvir-techbro/tesseract
##
## Copyright (c) 2026-present tanvir-techbro (Tanvir)
## Released under the MIT License.
##

# Developer tools (lexdump, ...). Built with the main project.

add_executable(lexdump
    LexDump.cpp
    ../Libreries/HtmlKit/src/Tokenize.cpp
)

set_target_properties(lexdump PROPERTIES OUTPUT_NAME LexDump)

target_include_directories(lexdump PRIVATE
    ../Libreries
)
