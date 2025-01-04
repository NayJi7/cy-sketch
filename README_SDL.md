<<<<<<< HEAD
# cy-sketch 

Pour le moment, Alice Franco et Jouaye Saghir travaillent sur cette partie en C en utilisant la bibliothèque SDL.
Cette partie permet de dessiner des formes dans une fenêtre à l'écran, et nous sommes actuellement en train de créer un curseur qui pourra : selectionner des formes, les deplacer, zoomer et rapetisser les formes ( cela nécessite donc de gérer les intéractions des utilisateurs). 

Actuellement, nous utilisons trois fichiers principaux :
main.c : gère la création de la fenêtre, le nettoyage , l'affichage => c'est le coeur du programme, le fichier qui relie tous les autres 
form.c : gère la création de formes ( cercle, ellipse, arc, rectangle ( avec angle droit ou angle arrondis ), polygones particuliers)  animées ou statiques et remplies ou uniquement le contour. 
De plus, il est possible de préciser la couleur et la position de la forme voulue.
clickEvents.c : gère tous les évenement utilisateurs ( clics de souris, de pavé etc)

Nous avons crée une fonction centrale, drawShape ( qu'on peut retrouver dans clickEvents.c ) qui prends les paraùètres suivants : la forme souhaitée, la position, la couleur, la taille, si il est remplie ou non et si il est animé ou non. Cette fonction oriente ensuite vers la fonction appropriée dans form.c pour générer la forme demandée.

Au départ, nous dessinions directement sur la fenêtre SDL. Cependant, lors de la création du curseur, nous avons constaté que ce dernier devait être géré sur une texture.  Travailler à la fois directement sur la fenêtre pour les formes et sur une texture pour le curseur créait donc un conflit. Nous avons donc dû reprendre nos fonction de création de formes afin de les adapter à une texture. Nous sommes donc encore en train de travailler pour adapter les codes entre eux. 
De plus, la création d'une texture nous a permis d'enlever des problèmes d'affichae "aléatoire" qu'on ne comprenait pas.
Nous avons reussi à créer un curseur, à lui donner une forme esthétique, à le déplacer, à pouvoir selectionner une forme, la déplacer, zoomer et rapeticir. Cependant, nous n'arrivons pas pour l'instant à l'implémenter sur les formes crée au préalable. Nous sommes donc en train de travailler sur cette partie la.

Pour lancer le code, il suffit de faire un make run et cela exécutera et la fenetre de dessin s'affichera.

ATTENTION : Avant de lancer le code il est important d'avoir installé au préalable la bibliothèque SDL2
=======
# Draw++ Compilator

Python-based compilator for a custom scripting language designed for creating simple graphical drawings. This interpreter converts Draw++ code into C code and executes it. It features file-based execution and an interactive mode for real-time drawing commands.

---

## Features

- **Lexical and Syntax Analysis**: Built using the PLY (Python Lex-Yacc) library.
- **AST Generation**: Parses the input into an abstract syntax tree (AST).
- **C Code Translation**: Converts AST nodes into C code for execution.
- **Interactive Mode**: Execute drawing commands in real-time.
- **Debugging Tools**: Debug mode to visualize tokens, AST, and translated C code.

---

## Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/NayJi7/cy-sketch
   cd cy-sketch
   ```

2. Install Python dependencies:
   ```bash
   pip install -r requirements.txt
   ```

---

## Requirements

- Python 3.6 or later
- Dependencies:
  - `ply` (Python Lex-Yacc)

---

## Usage

### **File-Based Execution**

Execute a `.dpp` source file:
```bash
python3 interpreter.py your_file.dpp
```

### **Interactive Mode**

Run the interpreter without specifying a file to enter interactive mode:
```bash
python3 interpreter.py
```

### **Debug Mode**

Enable debug mode with the `-d` flag for detailed output:
```bash
python3 interpreter.py -d your_file.dpp
```

---

## Interactive Mode Commands

- **Run Commands**: Enter any valid Draw++ command.
- **Clear Screen**: Type `c`.
- **Restart Interpreter**: Type `r`.
- **Exit**: Type `e` or `q`.

---

## Draw++ Language Overview

### **Keywords**

| Command   | Description                   |
|-----------|-------------------------------|
| `DRAW`    | Draw a shape                  |
| `MOVE`    | Move to a position            |
| `ROTATE`  | Rotate by an angle            |
| `COLOR`   | Set the drawing color         |
| `IF`      | Conditional execution         |
| `WHILE`   | Loop with a condition         |
| `FOR`     | Loop with an initialization   |
| `VARIABLE`| Declare a variable            |

### **Supported Shapes**

| Shape    | Description                   |
|----------|-------------------------------|
| `LINE`   | Draws a line                  |
| `CIRCLE` | Draws a circle                |
| `SQUARE` | Draws a square                |
| `ARC`    | Draws an arc                  |
| `POINT`  | Draws a point                 |

### **Example Program**
```draw++
var x = 10;
var y = 20;

color("red");
draw circle(x, y);
move(x, y);
rotate(45);
draw square(30, 30);
```

---

## Project Structure

```
- CY-SKETCH
├── src/
├────── lexer.py          # Lexical analyzer (tokenizes the input)
├────── parser.py         # Syntax analyzer (generates the AST)
├────── interpreter.py    # Main interpreter script
├────── myast.py          # AST utilities and execution functions
|
├── doc/
├────── generated/
├────────────────  compilator.pdf    # PDF doc generated with doxygen
├────────────────  compilator.html   # HTML doc generated with doxygen
|
├── Doxyfile          # Configuration file for doxygen
├── requirements.txt  # Txt file that contains all python requirements
└── README.md         # Readme file
```

---

## Debugging

Enable debug mode to view:
- Tokens generated by the lexer
- Abstract syntax tree (AST)
- Translated C code

Run the interpreter with `-d`:
```bash
python3 interpreter.py your_file.dpp -d
```

---

## Authors

- **Adam Terrak**
- **Deulyne Destin**

---

## Acknowledgments

- Thanks to the contributors and users who support this project.
- Built using the **PLY** library for Python.
>>>>>>> e7a5fa5f1a00effaabc8ae8ac9db964467511bb1
