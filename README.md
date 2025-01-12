# Draw++ (Cy-Sketch)

Un environnement de développement intégré pour créer des animations graphiques interactives avec un langage simple et intuitif.

## 📝 Description

Draw++ est composé de trois modules principaux qui travaillent ensemble :

### 🎨 Module SDL (Moteur Graphique)
- Gestion des formes géométriques (cercle, rectangle, ligne, polygone, etc.)
- Système d'animations avancé (rotation, zoom, couleur, rebond)
- Gestion des collisions et interactions
- Deux modes de jeu : Escape et Defense
- Interface utilisateur adaptative

### 🔧 Module Compilateur
- Lexer et Parser basés sur PLY
- Traduction du code Draw++ vers C
- Gestion intelligente des erreurs avec suggestions
- Support des variables et fonctions
- Mode interactif et mode fichier
- Mode debug pour le développement

### 💻 Module IDE
- Interface moderne avec coloration syntaxique
- Multi-fenêtres et multi-onglets
- Terminal intégré avec retours colorés
- Recherche et remplacement avancés
- Gestion des processus et sauvegarde automatique
- Barre d'outils personnalisable

## 🚀 Installation

### Prérequis
- Python 3.6+
- SDL2 et ses extensions (SDL2_gfx, SDL2_ttf)
- GCC (pour la compilation C)
- PyQt5 (pour l'IDE)

### Installation
1. Cloner le repository :
```bash
git clone https://github.com/votre-repo/Draw++.git
cd Draw++
```

2. Installer les dépendances :
- Sur Ubuntu :
```bash
./setup.sh
```
- Sur Windows :
```
Exécuter setup.bat en tant qu'administrateur
```

## 🎮 Utilisation

1. Lancer l'IDE :
```bash
cd Draw++
python3 ide.py
```

2. Créer un nouveau fichier .dpp
3. Écrire votre code Draw++ (voir Example.dpp)
4. Appuyer sur F5 pour exécuter

### Exemple de code :
```
draw circle(animated, filled, red, 100, 100, 50)
draw rectangle(instant, empty, blue, 200, 200, 100, 50)
set window color(black)
set window title("Draw++")
set window size(800, 600)
```

## 🎯 Fonctionnalités Principales

### Formes Supportées
- Cercle, Rectangle, Carré
- Triangle, Arc, Ellipse
- Ligne, Polygone (3-12 côtés)

### Animations
- Rotation (touche 'q' - gauche et 'd' - droite)
- Zoom (touches '*') / Dezoom (touche '/')
- Rotation, Changement de couleur, Rebond (style DVD) : touche 'a'

### Modes de Jeu
1. **EscapeShapes** : Capturez les formes qui fuient
2. **DefenseShapes** : Protégez vos formes des projectiles

## 👥 Contributeurs

- **MATIAS VINKOVIC** - *IDE et Interface*
- **ADAM TERRAK & DEULYNE DESTIN** - *Compilateur*
- **ALICE FRANCO & LOUAYE SAGHIR** - *Moteur SDL*

## 📚 Documentation

Consultez les READMEs spécifiques pour plus de détails :
- [Documentation SDL](SDL/README_SDL.md)
- [Documentation Compilateur](COMPILATOR/README.md)
