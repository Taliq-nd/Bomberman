#!/usr/bin/env bash
# make.sh — Build script for ASCII Bomberman (MSYS2 / Linux / macOS)
# Usage: bash make.sh

set -e

OUT="bomberman"
SRC=$(find src -name "*.c")

# Detect platform
if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" || "$OS" == "Windows_NT" ]]; then
    # Windows via MSYS2
    RAYLIB_INC="C:/msys64/mingw64/include"
    RAYLIB_LIB="C:/msys64/mingw64/lib"
    EXTRA_LIBS="-lraylib -lopengl32 -lgdi32 -lwinmm"
    OUT="bomberman.exe"
else
    # Linux / macOS (raylib installed via package manager)
    RAYLIB_INC=""
    RAYLIB_LIB=""
    EXTRA_LIBS="-lraylib -lGL -lm -lpthread -ldl -lrt -lX11"
fi

echo "Building ASCII Bomberman..."

if [ -n "$RAYLIB_INC" ]; then
    gcc $SRC -o "$OUT" -Isrc -I"$RAYLIB_INC" -L"$RAYLIB_LIB" $EXTRA_LIBS -std=c99 -Wall
else
    gcc $SRC -o "$OUT" -Isrc $EXTRA_LIBS -std=c99 -Wall
fi

echo "Build successful! -> $OUT"
