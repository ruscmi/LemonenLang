#!/bin/bash
set -e
BUILD_DIR="build"
INSTALL_PREFIX="$HOME/.local"
SYSTEM_INSTALL=false
while [ $# -gt 0 ]; do
    case "$1" in
        --system)
            SYSTEM_INSTALL-true
            shift
            ;;
        --clean)
            echo "clean old builder"
            rm -rf "$BUILD_DIR"
            shift
            ;;
        *)
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
    if [[ ":$PATH" != *":$HOME/.local/bin:"* ]]; then
        echo ""
        echo -e "\033[1;33mWarn: $HOME/.local/bin os not in your fucking PATH\033[0m"
        echo "Run this command to fix it: "
        echo " echo 'export PATH=\"\$HOME/.local/bin:\$PATH\"' >> ~/.bashrc && source ~/.bashrc or if you use zsh: ~/.zshrc"
    fi
fi
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
	if ! id -nG "$USER" | grep -qw "input"; then
	echo -e "\033[1;33mWarn: For keypressed func on system lib give rights"
	echo "Run this command: "
	echo "sudo usermod -aG input $USER"
	fi
fi
if [ "$NEED_SETCAP" = "1" ] || ! id -nG "$USER" | grep -qw "input"; then
    sudo setcap cap_dac_read_search=+ep ./lmnlang >/dev/null 2>&1
fi
echo "succefull"
