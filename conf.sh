#!/bin/bash
mkdir build
mkdir build/32
mkdir build/64
mkdir build/dxvk
mkdir build/dxvk/x64
mkdir build/dxvk/x32

DXVK_DIR="$PWD/build/dxvk/"
cd dxvk-uwp
meson setup --cross-file build-win64.txt --buildtype release --prefix "$DXVK_DIR/x64" build.w64
meson setup --cross-file build-win32.txt --buildtype release --prefix "$DXVK_DIR/x32" build.w32
cd ..

cd build/64
../../configure CC="ccache gcc" CROSSCC="ccache x86_64-w64-mingw32-gcc" --enable-win64
cd ../..

cd build/32
PKG_CONFIG_PATH=/usr/lib32 ../../configure CC="ccache gcc" CROSSCC="ccache i686-w64-mingw32-gcc" --with-wine64=../64
cd ../..
