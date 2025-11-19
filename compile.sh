#!/bin/bash

echo "[ 👾 ] Configuring Variables"
set -e

# Set VCPKG root

echo "[ 🌱 ] Configuring VCPKG Root"
VCPKG_ROOT="/home/frieren/vcpkg"

# Create build dir and move into it

echo "[ 🌳 ] Creating The Build Directory"
rm -rf build
mkdir -p build

echo "[ 📁 ] Moving Into The Build Directory"
cd build

# Configure with toolchain

echo "[ ⛓️ ] Configuring With Toolchain"
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
    -DCMAKE_BUILD_TYPE=Debug

# Build the project

echo "[ 🚧 ] Building The Project"

cmake --build .

echo "[ 💯 ] Successfully Built The Project"