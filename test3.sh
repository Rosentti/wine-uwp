#!/bin/bash
set -e

./build.sh
mkdir -p "$PWD/prefix"
WINEPREFIX="$PWD/prefix" ./build/64/wine /home/onni/Downloads/MarbleMaze_VS2017/MarbleMaze_VS2017/MarbleMaze_VS2017.exe
