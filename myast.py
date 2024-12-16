import os
import sys

# === 1. Gestion des erreurs ===
def print_error(error):
    """Affiche un message d'erreur formaté."""
    print(f"\033[31mErreur : {error}\033[0m")

# === 2. Traduction de l'AST vers C ===
def translate_ast_to_c(ast):
    """Traduire l'AST en code C."""
    c_code = ""

    # Ajout des includes nécessaires pour les bibliothèques C (remarque : à ajouter dans le fichier C généré)
    c_code += "#include <stdio.h>\n"
    c_code += "#include <stdlib.h>\n"
    c_code += "#include <math.h>\n"  # Si tu utilises des fonctions mathématiques comme 'sin', 'cos', etc.
    c_code += "\n"

    for node in ast:
        # Cas d'une instruction de dessin
        if isinstance(node, tuple) and node[0] == 'draw':
            forme = node[1]
            parametres = node[2]
            c_code += f"draw_{forme}({', '.join(map(str, parametres))});\n"
        
        # Cas d'un déplacement
        elif isinstance(node, tuple) and node[0] == 'move':
            x = node[1]
            y = node[2]
            c_code += f"move_to({x}, {y});\n"
        
        # Cas d'une rotation
        elif isinstance(node, tuple) and node[0] == 'rotate':
            angle = node[1]
            c_code += f"rotate({angle});\n"
        
        # Cas d'une assignation
        elif isinstance(node, tuple) and node[0] == 'assign':
            var_name = node[1]
            value = node[2]
            c_code += f"{type(value).__name__} {var_name} = {value};\n"
        
        # Cas d'une conditionnelle (if)
        elif isinstance(node, tuple) and node[0] == 'if':
            condition = node[1]
            bloc_true = node[2]
            bloc_false = node[3] if len(node) == 4 else None

            c_code += f"if ({condition}) {{\n{bloc_true}\n}}"
            if bloc_false:
                c_code += f"else {{\n{bloc_false}\n}}"
            c_code += "\n"
        
        # Cas d'une boucle (for)
        elif isinstance(node, tuple) and node[0] == 'for':
            init = node[1]
            condition = node[2]
            increment = node[3]
            bloc = node[4]
            c_code += f"for ({init}; {condition}; {increment}) {{\n{bloc}\n}}\n"
        
        # Cas d'un bloc
        elif isinstance(node, tuple) and node[0] == 'block':
            bloc = node[1]
            c_code += f"{{\n{bloc}\n}}\n"
        
        # Cas d'une couleur (assignation de couleur)
        elif isinstance(node, tuple) and node[0] == 'color':
            color = node[1]
            c_code += f"set_color({color});\n"
        
        # Cas par défaut : on ne sait pas quel type est ce nœud, donc le code est ignoré
        else:
            c_code += f"// Node non pris en charge : {node}\n"
    
    return c_code

# === 3. Exécution de l'AST ===
def execute_ast(ast, isgood):
    """Exécute l'AST généré par le parser."""
    if not ast:
        print("Aucune instruction à exécuter.")
        return
    
    # Traduction de l'AST en code C
    c_code = translate_ast_to_c(ast)

    filename = "generated_code.c"
    
    # Crée le fichier s'il n'existe pas
    if not os.path.exists(filename):
        with open(filename, 'w') as f:
            pass
    else:
        with open(filename, 'w') as f:
            pass

    # Écriture du code C dans le fichier
    if(isgood):
        with open(filename, 'a') as f:
            f.write(c_code)