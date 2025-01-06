#!/bin/bash

# Ensures the script is not run with sudo for PyInstaller-specific steps
if [ "$EUID" -eq 0 ]; then
  echo "Run this script without sudo for PyInstaller. Relaunch without: sudo $0"
  exit 1
fi

# Detects the operating system
if [[ "$OSTYPE" != "linux-gnu"* ]]; then
  echo "This script is designed for Ubuntu/Debian systems."
  exit 1
fi

# Determines the base path from the script's location
BASE_DIR=$(realpath $(dirname $(realpath $0))/../../)
IDE_DIR="$BASE_DIR/Draw++/IDE"
SDL_DIR="$BASE_DIR/Draw++/SDL"

# Function to install required system packages
install_dependencies() {
  echo "Updating package list..."
  sudo timeout 30 apt-get update || echo "Warning: apt-get update timed out, continuing the script..."

  echo "Installing required packages..."
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
}

# Function to install Python dependencies
install_python_dependencies() {
  echo "Installing Python dependencies..."
  pip3 install --break-system-packages -r requirements.txt freetype-py PyInstaller || echo "Some packages are already installed or system-managed."
}

# Function to verify and include resources
include_resources() {
  echo "Checking and including resources..."
  RESOURCES=(
    "$IDE_DIR/Dpp_circle.ico"
    "$IDE_DIR/.history"
    "$SDL_DIR/fonts/Consolas.ttf"
  )
  for resource in "${RESOURCES[@]}"; do
    if [ ! -f "$resource" ]; then
      echo "Missing resource: $resource"
      if [[ "$resource" == *".history" ]]; then
        echo "Creating an empty .history file to proceed..."
        touch "$IDE_DIR/.history"
      else
        exit 1
      fi
    fi
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

  python3 -m PyInstaller \
    --clean \
    --onefile \
    --name="DrawStudioCode" \
    --add-data "$IDE_DIR:DPP/IDE" \
    --add-data "$IDE_DIR/Dpp_circle.ico:DPP/IDE" \
    --add-data "$IDE_DIR/.history:DPP/IDE" \
    --add-data "$SDL_DIR/fonts/Consolas.ttf:DPP/IDE" \
    --hidden-import=PIL \
    --hidden-import=PIL._imagingtk \
    --hidden-import=PIL._tkinter_finder \
    --hidden-import=PyQt5 \
    --hidden-import=PyQt5.sip \
    --hidden-import=PyQt5.QtCore \
    --hidden-import=PyQt5.QtGui \
    --hidden-import=PyQt5.QtWidgets \
    Draw++/ide.py

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
read -p "Have you already done this setup once? (y/n) " response
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
