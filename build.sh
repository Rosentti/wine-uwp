#!/bin/bash
set -e

cd build/64
make -j24
cd ../..

cd build/32
make -j24
cd ../..
