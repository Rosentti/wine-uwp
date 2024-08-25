#!/bin/bash
rm -rf build/

mkdir build
mkdir build/32
mkdir build/64
mkdir build/dxvk
mkdir build/dxvki
mkdir build/dxvk/x64
mkdir build/dxvk/x32

./conf_dxvk.sh

cd build/64
../../configure CC="ccache gcc" CROSSCC="ccache x86_64-w64-mingw32-gcc" --enable-win64
cd ../..

cd build/32
PKG_CONFIG_PATH=/usr/lib32 ../../configure CC="ccache gcc" CROSSCC="ccache i686-w64-mingw32-gcc" --with-wine64=../64
cd ../..
