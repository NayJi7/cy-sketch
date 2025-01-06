#!/bin/bash

# Vérifie si le script est exécuté avec sudo uniquement pour les étapes nécessaires
if [ "$EUID" -eq 0 ]; then
  echo "Exécuter ce script sans sudo pour PyInstaller. Relancez sans : sudo $0"
  exit 1
fi

# Détecte le système d'exploitation
if [[ "$OSTYPE" != "linux-gnu"* ]]; then
  echo "Ce script est conçu pour les systèmes Ubuntu/Debian."
  exit 1
fi

# Détermine le chemin de base à partir de l'emplacement du script
BASE_DIR=$(realpath $(dirname $(realpath $0))/../../)
IDE_DIR="$BASE_DIR/Draw++/IDE"
SDL_DIR="$BASE_DIR/Draw++/SDL"

# Fonction pour installer les paquets système requis
install_dependencies() {
  echo "Mise à jour de la liste des paquets..."
  sudo timeout 30 apt-get update || echo "Avertissement : apt-get update a expiré, le script continue..."

  echo "Installation des paquets nécessaires..."
  PACKAGES=(
    build-essential gcc python3 python3-pip libsdl2-2.0-0 libsdl2-dev libsdl2-gfx-dev libsdl2-ttf-dev qt5-default
  )
  for pkg in "${PACKAGES[@]}"; do
    if ! dpkg -l | grep -q "^ii  $pkg "; then
      echo "Installation de $pkg..."
      sudo apt-get install -y "$pkg"
    else
      echo "$pkg est déjà installé."
    fi
  done
}

# Fonction pour installer les dépendances Python
install_python_dependencies() {
  echo "Installation des dépendances Python..."
  pip3 install --break-system-packages -r requirements.txt freetype-py PyInstaller || echo "Certains paquets sont déjà installés ou gérés par le système."
}

# Fonction pour configurer les ressources
include_resources() {
  echo "Vérification et inclusion des ressources..."
  RESOURCES=(
    "$IDE_DIR/Dpp_circle.ico"
    "$IDE_DIR/.history"
    "$SDL_DIR/fonts/Consolas.ttf"
  )
  for resource in "${RESOURCES[@]}"; do
    if [ ! -f "$resource" ]; then
      echo "Ressource manquante : $resource"
      if [[ "$resource" == *".history" ]]; then
        echo "Création d'un fichier .history vide pour continuer..."
        touch "$IDE_DIR/.history"
      else
        exit 1
      fi
    fi
  done
}

# Fonction pour configurer l'intégration Linux (menu desktop)
setup_desktop_integration() {
  echo "Copie de l'icône dans le dossier des icônes utilisateur..."
  mkdir -p ~/.local/share/icons/
  cp "$IDE_DIR/Dpp_circle.ico" ~/.local/share/icons/Dpp_circle.ico || {
    echo "Erreur : Impossible de copier l'icône. Vérifiez son existence."
    exit 1
  }
  echo "Configuration de l'intégration Linux..."
  cat > DrawStudioCode.desktop << EOL
[Desktop Entry]
Version=1.0
Type=Application
Name=DrawStudioCode
Comment=Draw++ Studio Code
Exec=$(pwd)/DrawStudioCode
Icon=~/.local/share/icons/Dpp_circle.ico
Terminal=false
Categories=Development;IDE;
EOL

  chmod +x DrawStudioCode.desktop
  mkdir -p ~/.local/share/applications/
  cp DrawStudioCode.desktop ~/.local/share/applications/
  echo "L'intégration au menu est terminée. Vous pouvez maintenant trouver Draw++ dans votre menu Applications."
}

# Fonction pour construire l'exécutable avec PyInstaller
build_executable() {
  echo "Construction de l'exécutable avec PyInstaller..."
  cd "$BASE_DIR" || exit 1

  # Supprime l'exécutable existant s'il existe
  if [ -f "DrawStudioCode" ]; then
    echo "Suppression de l'exécutable existant..."
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

  echo "Déplacement des fichiers de build dans Draw++/IDE..."
  mkdir -p "$IDE_DIR/build"
  cp -r build/* "$IDE_DIR/build/" || true
  mv DrawStudioCode.spec "$IDE_DIR/" || true
  rm -rf build || true

  echo "Déplacement de l'exécutable au niveau supérieur..."
  mv dist/DrawStudioCode . || true
  rm -rf dist || true

  # Rendre l'exécutable exécutable
  chmod +x DrawStudioCode

  # Rendre l'historique accessible en écriture
  rm -rf Draw++/IDE/.history
  touch Draw++/IDE/.history

  # Exporter la variable d'environnement pour Qt
  export QT_QPA_PLATFORM=xcb
}

# Lancement des fonctions
install_dependencies
install_python_dependencies
include_resources
build_executable
setup_desktop_integration

echo "Installation terminée avec succès ! Vous pouvez exécuter l'application avec ./DrawStudioCode."