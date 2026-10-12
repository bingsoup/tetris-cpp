#!/usr/bin/env bash
set -euo pipefail

cd /workspaces/tetris-cpp

echo "Configuring Tetris..."

rm -rf build-test

cmake \
  -S . \
  -B build-test \
  -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=/usr/local/vcpkg/scripts/buildsystems/vcpkg.cmake \
  -DVCPKG_MANIFEST_MODE=ON

echo "Building Tetris..."

cmake --build build-test

echo "Setup complete."