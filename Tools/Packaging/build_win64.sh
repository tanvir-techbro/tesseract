#!/bin/sh
# Cross-compile tesseract for Windows x64 (MinGW) and stage dist-win/win64/.
# Usage: ./build_win64.sh [SDL_PREFIX] [SDLTTF_PREFIX]
#   SDL_PREFIX / SDLTTF_PREFIX: unpacked SDL3 / SDL3_ttf mingw dev packs.
#   Falls back to $SDL_PREFIX / $SDLTTF_PREFIX env, then ./thirdparty/*.
# Output: dist-win/win64/ + tesseract-win64.zip
set -e
# Script lives in Tools/Packaging/; everything else is repo-root relative.
REPO_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$REPO_ROOT"

SDL_PREFIX="${1:-${SDL_PREFIX:-./thirdparty/SDL3}}"
SDLTTF_PREFIX="${2:-${SDLTTF_PREFIX:-./thirdparty/SDL3_ttf}}"
BUILD_DIR="${BUILD_DIR:-build-win}"
OUT="dist-win/win64"

command -v x86_64-w64-mingw32-g++ >/dev/null || {
      echo "need mingw-w64-gcc (x86_64-w64-mingw32-g++)" >&2
      exit 1
}
for d in "$SDL_PREFIX" "$SDLTTF_PREFIX"; do
      [ -d "$d" ] || {
            echo "missing SDL pack: $d (pass paths as args)" >&2
            exit 1
      }
done

cmake -B "$BUILD_DIR" -S . --toolchain Toolchains/mingw-x64.cmake \
      -DCMAKE_BUILD_TYPE=Release \
      -DSDL_PREFIX="$SDL_PREFIX" -DSDLTTF_PREFIX="$SDLTTF_PREFIX"
cmake --build "$BUILD_DIR" --target tesseract

rm -rf "$OUT"
mkdir -p "$OUT"
cp "$BUILD_DIR/tesseract.exe" "$OUT/"

# SDL runtime DLLs from the dev packs.
cp "$SDL_PREFIX/x86_64-w64-mingw32/bin/SDL3.dll" "$OUT/"
cp "$SDLTTF_PREFIX/x86_64-w64-mingw32/bin/SDL3_ttf.dll" "$OUT/"

# MinGW runtime DLLs: search compiler-adjacent dirs, then -print-file-name.
CC_BIN="$(dirname "$(command -v x86_64-w64-mingw32-g++)")"
find_dll() {
      for dir in "$CC_BIN/../x86_64-w64-mingw32/bin" "$CC_BIN"; do
            if [ -f "$dir/$1" ]; then
                  echo "$dir/$1"
                  return 0
            fi
      done
      p="$(x86_64-w64-mingw32-gcc -print-file-name="$1")"
      case "$p" in
      /*) echo "$p" ;;
      *) return 1 ;;
      esac
}
for dll in libstdc++-6.dll libgcc_s_seh-1.dll libwinpthread-1.dll; do
      path="$(find_dll "$dll")" || {
            echo "missing runtime $dll" >&2
            exit 1
      }
      cp "$path" "$OUT/"
done

cp -r Assets "$OUT/"
cp README.md LICENSE "$OUT/"

rm -f tesseract-win64.zip
(cd dist-win && zip -qr ../tesseract-win64.zip win64/)
echo "ready: $OUT/ + tesseract-win64.zip"
