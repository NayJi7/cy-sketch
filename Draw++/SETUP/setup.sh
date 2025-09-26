#!/bin/bash

# Ensures the script is not run with sudo for PyInstaller-specific steps
if [ "$EUID" -eq 0 ]; then
  echo "Run this script without sudo for PyInstaller. Relaunch without: sudo $0"
  exit 1
fi

# Detects the operating system
if [[ "$OSTYPE" != "linux-gnu"* ]]; then
  echo "This script is designed for Linux systems."
  # continue but warn
fi

# Determines the base path from the script's location
BASE_DIR=$(realpath $(dirname $(realpath $0))/../../)
IDE_DIR="$BASE_DIR/Draw++/IDE"
SDL_DIR="$BASE_DIR/Draw++/SDL"
COMPILATOR_DIR="$BASE_DIR/Draw++/COMPILATOR"

# Initialize PyInstaller arguments
PYINSTALLER_ARGS="--clean --onefile --name=DrawStudioCode"

# Function to install required system packages
install_dependencies() {
  echo "Installing required system packages..."

  # Detect package manager and install accordingly (Debian/Ubuntu or Arch)
  if command -v apt-get >/dev/null 2>&1; then
    echo "Detected apt-get (Debian/Ubuntu). Updating package list..."
    sudo timeout 30 apt-get update || echo "Warning: apt-get update timed out, continuing the script..."
    PACKAGES=(
      build-essential gcc python3 python3-pip libsdl2-2.0-0 libsdl2-dev libsdl2-gfx-dev libsdl2-ttf-dev qt5-default
    )
    for pkg in "${PACKAGES[@]}"; do
      if ! dpkg -l | grep -q "^ii  $pkg "; then
        echo "Installing $pkg..."
        sudo apt-get install -y "$pkg"
      else
        echo "$pkg is already installed."
      fi
    done

  elif command -v pacman >/dev/null 2>&1; then
    echo "Detected pacman (Arch/Manjaro). Installing packages with pacman..."
    # Map Debian package names to pacman equivalents where possible
    PACMAN_PKGS=(
      base-devel gcc python python-pip sdl2 sdl2_gfx sdl2_ttf qt5-base
    )
    # Use --needed to skip already-installed packages
    sudo pacman -Sy --needed --noconfirm "${PACMAN_PKGS[@]}"

  else
    echo "Unsupported package manager. Please install build-essential (or base-devel), gcc, python3, python3-pip, SDL2, SDL2_gfx, SDL2_ttf and Qt5 manually." 
    echo "On Arch: sudo pacman -S --needed base-devel gcc python python-pip sdl2 sdl2_gfx sdl2_ttf qt5-base"
  fi
}

# Function to install Python dependencies
install_python_dependencies() {
  echo "Installing Python dependencies..."

  # Workaround for potential PyInstaller version conflicts: prefer the pinned version in requirements.txt
  # If requirements.txt pins pyinstaller, let pip install it; otherwise install requirements without adding PyInstaller twice.
  # Some environments (e.g. Python 3.13 on Arch) may not have wheels for older PyInstaller or PyQt5.
  # Strategy: install requirements excluding PyInstaller, then install a PyInstaller 6.x compatible with newer Pythons.
  if grep -q -i "pyinstaller" requirements.txt; then
    echo "requirements.txt contains PyInstaller. Installing everything except PyInstaller and readline first..."
    # Create a temporary requirements file without pyinstaller and readline lines which often fail to build on Linux
    TMP_REQS=$(mktemp)
    grep -iv -E "^pyinstaller|^readline" requirements.txt > "$TMP_REQS" || true
    pip3 install --break-system-packages -r "$TMP_REQS" || echo "Some packages are already installed or system-managed."
    rm -f "$TMP_REQS"
    # Install a PyInstaller version compatible with newer Pythons (6.16.0 is known available)
    echo "Installing PyInstaller 6.16.0 (compatible with newer Python versions)..."
    pip3 install --break-system-packages pyinstaller==6.16.0 || echo "Could not install pyinstaller via pip; consider using a virtualenv or installing pyinstaller from your distro."
  else
    echo "requirements.txt does not contain PyInstaller; installing requirements then latest PyInstaller (excluding readline)..."
    TMP_REQS=$(mktemp)
    grep -iv -E "^readline" requirements.txt > "$TMP_REQS" || true
    pip3 install --break-system-packages -r "$TMP_REQS" freetype-py PyInstaller || echo "Some packages are already installed or system-managed."
    rm -f "$TMP_REQS"
  fi
}

# Function to verify and include resources
include_resources() {
  echo "Checking and including resources..."
  
  # Define the resources
  RESOURCES=(
    "$BASE_DIR/Draw++/Makefile"
    "$BASE_DIR/Draw++/interpreter.py"
    "$IDE_DIR/*.ico"
    "$IDE_DIR/*.svg"
    "$IDE_DIR/*.png"
    "$IDE_DIR/*.bmp"
    "$SDL_DIR/fonts/*.ttf"
    "$SDL_DIR/files.h/*.h"
    "$SDL_DIR/src/*.c"
    "$COMPILATOR_DIR/src/*.py"
  )

  for RESOURCE in "${RESOURCES[@]}"; do
    # Skip empty resources
    [[ -z "$RESOURCE" ]] && continue

    # Expand wildcards into individual files
    for FILE in $(find $(dirname "$RESOURCE") -name "$(basename "$RESOURCE")" 2>/dev/null); do
      if [[ -f "$FILE" ]]; then
        # Make DEST relative to the Draw++ directory so PyInstaller receives a relative destination
        REL_PATH=${FILE#${BASE_DIR}/Draw++/}
        DEST_DIR=$(dirname "$REL_PATH")
        if [[ "$REL_PATH" == "interpreter.py" || "$REL_PATH" == "Makefile" ]]; then
          DEST_DIR="."
        fi
        # If dirname gives '.' or empty, keep '.'
        if [[ -z "$DEST_DIR" ]]; then
          DEST_DIR="."
        fi
        echo "Including file: $FILE -> $DEST_DIR"
        PYINSTALLER_ARGS+=" --add-data $FILE:$DEST_DIR"
      fi
    done
  done
}

# Function to configure Linux integration (desktop menu)
setup_desktop_integration() {
  echo "Copying the icon to the user icon folder..."
  mkdir -p ~/.local/share/icons/
  cp "$IDE_DIR/Dpp_circle.svg" ~/.local/share/icons/Dpp_circle.svg || {
    echo "Error: Unable to copy the icon. Please check its existence."
    exit 1
  }
  echo "Setting up Linux desktop integration..."
  cat > DrawStudioCode.desktop << EOL
[Desktop Entry]
Version=1.0
Type=Application
Name=DrawStudioCode
Comment=Draw++ Studio Code
Exec=$(pwd)/DrawStudioCode
Icon=Dpp_circle.svg
Terminal=false
Categories=Development;IDE;
EOL

  chmod +x DrawStudioCode.desktop
  mkdir -p ~/.local/share/applications/
  mv DrawStudioCode.desktop ~/.local/share/applications/
  echo "Desktop menu integration complete. You can now find Draw++ in your Applications menu."
}

# Function to build the executable with PyInstaller
build_executable() {
  echo "Building the executable with PyInstaller..."
  cd "$BASE_DIR" || exit 1

  # Remove existing executable if it exists
  if [ -f "DrawStudioCode" ]; then
    echo "Removing existing executable..."
    rm -f DrawStudioCode
  fi

  python3 -m PyInstaller $PYINSTALLER_ARGS \
    --hidden-import=PIL \
    --hidden-import=PIL._imagingtk \
    --hidden-import=PIL._tkinter_finder \
    --hidden-import=PyQt5 \
    --hidden-import=PyQt5.sip \
    --hidden-import=PyQt5.QtCore \
    --hidden-import=PyQt5.QtGui \
    --hidden-import=PyQt5.QtWidgets \
    --name DrawStudioCode \
    "$BASE_DIR/Draw++/ide.py"

  echo "Moving build files to Draw++/IDE..."
  mkdir -p "$IDE_DIR/build"
  cp -r build/* "$IDE_DIR/build/" || true
  mv DrawStudioCode.spec "$IDE_DIR/" || true
  rm -rf build || true

  echo "Moving executable to the top-level directory..."
  mv dist/DrawStudioCode . || true
  rm -rf dist || true

  # Make the executable runnable
  chmod +x DrawStudioCode

  # Make history file writable
  rm -rf Draw++/IDE/.history
  touch Draw++/IDE/.history

  # Export environment variable for Qt
  export QT_QPA_PLATFORM=xcb
}

# Ask the user if they already have the necessary dependencies
read -p "Have you already done this setup once? Skip dependencies installation? (y/n) " response
if [[ "$response" =~ ^[yY]$ ]]; then
    echo "Skipping dependency installation."
else
    install_dependencies
    install_python_dependencies
fi

# Include resources, build the executable, and configure desktop integration
include_resources
build_executable
setup_desktop_integration

echo " "
echo "Installation completed successfully! - You can run the application with ./DrawStudioCode in the project directory."
echo "                                     - You can also run it from the Applications menu (DrawStudioCode)."