#!/usr/bin/env bash

set -euo pipefail

cd /workspaces/tetris-cpp

echo "Configuring tetris"

rm -rf build

VCPKG_BINARY_SOURCES=clea cmake \
    -S . \
    -B build \
    -G Ninja \

-DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" 

echo "CMake configured"