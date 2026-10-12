#!/usr/bin/env bash

set -euo pipefail

cd /workspaces/tetris-cpp

echo "VCPKG_ROOT=$VCPKG_ROOT"
echo "installing vcpkg"

test -f vcpkg.json
test -f "$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"

vcpkg install \
    --x-manifest-root="$PWD" \
    --binarysource=clear

echo "checking SDL3"

find "$PWD/vcpkg_installed" -type f -name "SDL3Config.cmake" -print

echo "Configuring"

rm -rf build

cmake \
    -S . \
    -B build \
    -G Ninja \

-DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
-DVCPKG_MANIFEST_MODE=ON \
-DVCPKG_MANIFEST_DIR="$PWD"

echo "CMake configured"