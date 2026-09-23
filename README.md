# tesseract

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
