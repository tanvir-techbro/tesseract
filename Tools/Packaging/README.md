# Windows packaging (from Linux)

Cross-compile with MinGW, stage `dist-win/win64/`, zip it.
Everything runs from the repo root; paths below are relative to it.

## One-time setup

1. Install the cross toolchain (Arch: `mingw-w64-gcc`, Debian: `g++-mingw-w64-x86-64`).
2. Download the SDL mingw dev packs and extract under `thirdparty/`:

```
mkdir -p thirdparty && cd thirdparty
curl -fSL -o sdl3.tgz https://github.com/libsdl-org/SDL/releases/download/release-3.4.16/SDL3-devel-3.4.16-mingw.tar.gz
curl -fSL -o sdlttf.tgz https://github.com/libsdl-org/SDL_ttf/releases/download/release-3.2.2/SDL3_ttf-devel-3.2.2-mingw.tar.gz
tar xzf sdl3.tgz && tar xzf sdlttf.tgz
cd ..
```

`thirdparty/` is gitignored — SDKs never enter the repo.

## Build + package

Pass the unpacked SDK dirs explicitly (nothing is stored; the exe
is identical however you point at them):

```
./Tools/Packaging/build_win64.sh thirdparty/SDL3-3.4.16 thirdparty/SDL3_ttf-3.2.2
# or: SDL_PREFIX=... SDLTTF_PREFIX=... ./Tools/Packaging/build_win64.sh
```

Output: `dist-win/win64/` (exe + SDL/MinGW DLLs + `Assets/`) and
`tesseract-win64.zip` (unzips to `win64/`). Both gitignored.

## Installer (on Windows)

`installer.iss` (repo root) + Inno Setup: `iscc installer.iss`
with `dist-win/win64/` beside it → `tesseract-setup.exe`.

## Notes

* The exe links subsystem-windows: no console spawns.
  Debug tools (`LexDump`, …) stay console apps.
* `Src/WinPrint.cpp` works around MinGW 16.x's broken
  `std::print` (missing terminal symbols). Recheck per toolchain bump.
* `Tools/Toolchains/mingw-x64.cmake` holds the cross config; pass
  `-DSDL_PREFIX` / `-DSDLTTF_PREFIX` to point at the SDKs.
