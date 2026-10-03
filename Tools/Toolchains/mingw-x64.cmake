# MinGW-w64 cross toolchain (Linux -> Windows x64).
# Usage:
#   1. Install mingw-w64-gcc.
#   2. Download SDL3 + SDL3_ttf mingw dev packs, extract somewhere.
#   3. cmake -B build-win -S . --toolchain Toolchains/mingw-x64.cmake \
#        -DSDL_PREFIX=/path/to/SDL3-3.4.16 -DSDLTTF_PREFIX=/path/to/SDL3_ttf-3.2.2
#   4. cmake --build build-win

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

# Pass -DSDL_PREFIX / -DSDLTTF_PREFIX at configure time.
set(SDL_ROOT_HINTS "${SDL_PREFIX}/x86_64-w64-mingw32" "${SDLTTF_PREFIX}/x86_64-w64-mingw32")
set(CMAKE_FIND_ROOT_PATH ${SDL_ROOT_HINTS})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
set(CMAKE_PREFIX_PATH "${SDL_PREFIX}/cmake;${SDLTTF_PREFIX}/cmake")
