#!/bin/bash
set -e
BUILD_DIR="build"
INSTALL_PREFIX="$HOME/.local"
SYSTEM_INSTALL=false
for arg in "$@"
do
    case $arg in
        --system)
        SYSTEM_INSTALL=true
        shift
        ;;
        --clean)
        echo "clean old builder"
        rm -rf "$BUILD_DIR"
        shift
        ;;
    esac
done
echo "
        lmnlang - lightweight and fast interpreter
        Copyright (C) 2026 ruscmi
        lmnlang is probided under:
        GNU V2.0 GENERAL PUBLIC LICENSE
"
echo "Configuring..."
if [ "$SYSTEM_INSTALL" = true ]; then
    cmake -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release
else
    cmake -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX"
fi
echo "Compilation lmnlang..."
cmake --build "$BUILD_DIR" -j$(nproc)
echo "Download..."
if [ "$SYSTEM_INSTALL" = true ]; then
    echo "need a fucking installation rights"
    sudo cmake --install "$BUILD_DIR"
else
    cmake --install "$BUILD_DIR"
fi
echo "succefull"
