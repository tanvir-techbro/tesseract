# Tesseract

A simple browser engine.

## Install

Needs CMake 3.20+, a C++23 compiler, SDL3 + SDL3_ttf.

```
cmake -B build -S .
cmake --build build
```

## Use

```
./build/tesseract
```

Type a `file://` URL or bare path in the URL bar, hit Enter.
Try `file://Tests/001_html1.html`.

Works now: `file://` pages with headings, paragraphs, and divs.
Debug tools build along: `build/Tools/LexDump`, `TreeDump`, `UrlDump`.

## Windows

End users: download `tesseract-windows-x64.zip` from Releases,
unzip, run `tesseract.exe`. No installs needed.

Developers (MSVC + vcpkg):

```
vcpkg install sdl3 sdl3-ttf
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=<vcpkg>/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release
```

Run from the repo root so `Assets/` resolves
(`build\Release\tesseract.exe` needs CWD = root).

Cross-compile from Linux (MinGW):

```
# one-time: mingw-w64-gcc + SDL3/SDL3_ttf mingw dev packs,
# toolchain file pointing at them (see build-win/ setup)
cmake -B build-win -S . --toolchain <mingw64.cmake> -DCMAKE_BUILD_TYPE=Release
cmake --build build-win
```

The GUI target links subsystem-windows: no console spawns.
Debug tools stay console apps.
