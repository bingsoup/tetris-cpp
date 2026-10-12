#!/usr/bin/env bash

set -e

echo "Configuring"

rm -rf build

cmake \
    -S . \
    -B build \
    -G Ninja \

-DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake

echo "CMake configured"