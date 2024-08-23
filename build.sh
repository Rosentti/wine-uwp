#!/bin/bash
set -e

# Begin with building dxvk-uwp
cd dxvk-uwp/build.w64
ninja install
cd ..
cd build.w32
ninja install
cd ../..

cd build/64
make -j24
cd ../..

cd build/32
make -j24
cd ../..
