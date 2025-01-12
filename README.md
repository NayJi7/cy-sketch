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

- Interface graphique moderne et intuitive
- Coloration syntaxique, et dynamique pour les attributs de couleurs
- Ouverture de fenêtres et onglets en simultané
- Terminal intégré, avec retour colorié en fonction de la nature de la sortie
- Raccourcis clavier intégrés
- Recherche/Remplacement
- Sauvegarde automatique
- Barre de menu amovible et modulable
- Possibilité de kill un process en cours

## 🚀 Installation

### Prérequis principaux (obtenus pendant le setup plus bas)

- Python 3.6+
- SDL2 et ses extensions (SDL2_gfx, SDL2_ttf)
- GCC (pour la compilation C)
- PyQt5 (pour l'IDE)

### Installation

1. Cloner le repository :

```bash
git clone https://github.com/NayJi7/cy-sketch
```

2. Installer les dépendances :

- Sur Ubuntu :

```bash
cd Draw++/SETUP
./setup.sh
```

- Sur Windows :

```
Aller dans Draw++/SETUP
Exécuter setup.bat en tant qu'administrateur
```

## 🎮 Utilisation

1. Lancer l'IDE :

```bash
Via python (dans Draw++/) : python3 ide.py
Lancer le .exe créé par le setup sur windows (à la racine du projet)
Lancer l`executable créé par le setup sur ubuntu (à la racine du projet ou dans les applications)
```

2. Créer un nouveau fichier .dpp
3. Écrire votre code Draw++ (voir Example.dpp)
4. Executez et profitez !

### Exemple de code :

```
draw circle(animated, filled, red, 100, 100, 50)
draw rectangle(instant, empty, blue, 200, 200, 100, 50)
set window color(black)
set window title("Draw++")
set window size(800, 600)
```

## 🎯 Fonctionnalités Principales

### Fonctions supportées

- draw

  - circle (mode, remplissage, couleur, centerX, centerY, radius)
  - rectangle (mode , remplissage , couleur , x, y, width ,height)
  - square (mode , remplissage , couleur , x, y, sideLength)
  - polygon (mode , remplissage , couleur , centerX , centerY ,radius , sides)
  - triangle (mode , remplissage , couleur , centerX , centerY ,radius)
  - arc (mode , couleur , centerX , centerY , radius ,startAngle , endAngle)
  - line (mode , couleur , x1 , y1 , x2 , y2 , thickness)
  - ellipse (mode , remplissage , couleur , centerX , centerY ,radiusX , radiusY)
- set

  - cursor
    - size(size)
    - color(color)
  - window
    - size(width , height)
    - color(color)

### Fonctionnalités de base

* Boucles
  * while ( condition )  { bloc }
  * do { bloc } while ( condition )
  * for ( initialize; condition; increment ) { bloc }
* Conditions
  * if ( condition ) { bloc }
  * elif ( condition ) { boc }
  * else ( condition ) { bloc }
* Déclaration et modification / utilisation de variables / fonctions
  * var name = value
  * name = value
  * func myfunction ( int a, float b, char c[size] ) { bloc }
  * myfunction (arguments)
* Commentaires
  * Single line : # commentaire
  * Multi line : /* commentaire */
* Opérateurs
  * Aritmétiques ( +, -, *, / )
  * Logiques ( or, and, ==, !=, <, >, <=, >= )
  * Booléens ( true, false )

### Interractions

- Rotation gauche ( q ) / droite ( d )
- Selection / Déselection d'une forme ( click )
- Bouger une forme ( avec le mouvement de la souris ou flèches directionnelles )
- Zoom ( * ) / Dezoom ( / ) ( molette )
- Selection d'animation ( + ) ( - )
- Application d'une animation ( enter )
- Retrait d'une animation ( effacer )
- Lancement de la liste d'animations ( a )
- Changement de supperposition des formes ( z ) ( s )
- Réinitialisation de la taille, zoom et couleur ( r )
- Arrêt de toutes les animations ( n )
- Suppression d'une forme ( suppr )
- Mode de jeu ( g )
  - Séléction du jeu ( g )
  - Quitter ( space )
  - Lancer le jeu ( enter )
- Quitter ( escape )

### Animations

Ces animations peuvent être appliquées en même temps et différement pour chaque forme ( 3 animations max par forme )

* Rotate
  * Une rotation
* Zoom
  * Un effet de zoom et dezoom
* Color
  * Un changement de couleurs
* Bounce
  * Un effet de rebond donné à la forme qui parcourira l'écran et rebondira sur les bords

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
