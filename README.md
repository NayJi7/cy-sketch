# Draw++

Bienvenue dans notre projet : Cy-Sketch ! 

Un projet graphique interactif permettant de créer et manipuler des formes en temps réel avec un langage de programmation simple.
Nous avons même mis en place 2 jeux : Escape et Defense Shapes.

## 🎨 Présentation

Draw++ est composé de trois parties principales :

### 1. IDE
- Interface graphique moderne et intuitive
- Coloration syntaxique, et dynamique pour les attributs de couleurs
- Ouverture de fenêtres et onglets en simultané
- Terminal intégré, avec retour colorié en fonction de la nature de la sortie
- Raccourcis clavier intégrés
- Recherche/Remplacement
- Sauvegarde automatique
- Barre de menu amovible et modulable
- Possibilité de kill un process en cours

### 2. COMPILATEUR
- Langage simple et intuitif
- Gestion des erreurs avec suggestions
- Support des variables et fonctions
- Compilation vers C
- Mode debug

### 3. SDL (Moteur graphique)
- Formes géométriques : cercle, rectangle, ligne, polygone, etc.
- Animations : rotation, zoom, couleur, rebond
- Interactions à la souris
- Sélection et manipulation des formes
- Modes de jeu intégrés 

## 🚀 Fonctionnalités

- Création de formes avec différents styles (plein/vide)
- Animations multiples combinables
- Interaction en temps réel
- Gestion des collisions
- Modes de jeu : Escape et Defense
- Personnalisation des couleurs et tailles
- Support des événements souris/clavier

## 💻 Installation

1. Cloner le repository
2. Installer les dépendances :
   ```./setup.sh``` (sur Ubuntu)
   ```Lancer en tant qu'administrateur le fichier setup.bat``` (sur Windows)

## 🎮 Utilisation

Lancer le programme  :
    ```cd Draw++```
    - avec l'IDE :
    ```python3 ide.py```
        a. Écrire votre code Draw++ (voir Example.dpp)
        b. Appuyer sur F5/Run pour exécuter

## 🛠️ Technologies

- Python (IDE et Compilateur)
- C (Moteur graphique SDL)
- SDL2 (Graphismes)
- PyQt5 (Interface)

## 👥 Contributeurs

- **[MATIAS VINKOVIC]** - *IDE et Interface*
- **[ADAM TERRAK & DEULYNE DESTIN]** - *Compilateur*
- **[ALICE FRANCO & LOUAYE SAGHIR]** - *Moteur SDL*
