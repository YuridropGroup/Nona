@echo off
setlocal enabledelayedexpansion

echo [ 👾 ] Configuring Variables

REM Set VCPKG root
echo [ 🌱 ] Configuring VCPKG Root
set VCPKG_ROOT=C:\vcpkg

REM Create build dir and move into it
echo [ 🌳 ] Creating The Build Directory
if not exist build (
    mkdir build
)

echo [ 📁 ] Moving Into The Build Directory
cd build

REM Configure with toolchain
echo [ ⛓️ ] Configuring With Toolchain
cmake .. ^
    -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake ^
    -DCMAKE_BUILD_TYPE=Debug

REM Build the project
echo [ 🚧 ] Building The Project
cmake --build .

echo [ 💯 ] Successfully Built The Project
