#!/bin/bash

# Detect OS
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # Ubuntu/Debian setup
    echo "Setting up dependencies for Ubuntu/Debian..."
    
    # Update package list
    sudo apt-get update
    
    # Install build essentials and SDL2 dependencies
    sudo apt-get install -y \
        build-essential \
        gcc \
        python3 \
        python3-pip \
        libsdl2-2.0-0 \
        libsdl2-dev \
        libsdl2-gfx-dev \
        libsdl2-ttf-dev

elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
    # Windows setup using MSYS2
    echo "Setting up dependencies for Windows..."
    echo "Please make sure MSYS2 is installed from https://www.msys2.org/"
    echo "Then run these commands in MSYS2 terminal:"
    echo ""
    echo "pacman -Syu --noconfirm"
    echo "pacman -S --noconfirm mingw-w64-x86_64-gcc"
    echo "pacman -S --noconfirm mingw-w64-x86_64-SDL2"
    echo "pacman -S --noconfirm mingw-w64-x86_64-SDL2_gfx"
    echo "pacman -S --noconfirm mingw-w64-x86_64-SDL2_ttf"
    echo "pacman -S --noconfirm python3"
    echo ""
    echo "Add these to your PATH:"
    echo "C:\msys64\mingw64\bin"
    echo "C:\msys64\usr\bin"
else
    echo "Unsupported operating system"
    exit 1
fi

# Install Python dependencies
pip3 install -r requirements.txt

echo "Setup complete!"
echo "You can now build the project using 'make' 