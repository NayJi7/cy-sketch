import os

# === 1. Gestion des erreurs ===
def print_error(error):
    """Affiche un message d'erreur formaté."""
    print(f"\033[31mErreur : {error}\033[0m")

def condition_to_c(condition):
    return f"{condition[1]} {condition[0]} {condition[2]}"

def find_variable_in_ast(var_name, ast_tree):
    """
    Search the AST for the declaration or assignment of a given variable name.
    Returns the type and value of the variable.
    """
    for node in ast_tree:
        if isinstance(node, tuple) and node[0] == 'assign':
            assigned_var_name = node[1]
            assigned_value = node[2]
            
            if assigned_var_name == var_name:
                # Determine the type of the value
                if isinstance(assigned_value, str) and '"' in assigned_value:
                    return "char*", assigned_value  # String case
                elif isinstance(assigned_value, (int, float)):
                    return type(assigned_value).__name__, assigned_value
                elif isinstance(assigned_value, str):  # Identifier case
                    return find_variable_in_ast(assigned_value, ast_tree)
    return None, None  # Variable not found

# === 2. Traduction des nodes vers C ===
def translate_node_to_c(ast,node, newline, tabulation, semicolon):
    # adapter la fonction pour ajouter ou non les newline, tab et semicolon
    # adapter chaque appel a cette fonction pour indiquer si on veut mettre newline, tab et semicolon.
    """Traduire une node en code C."""
    c_code = ""

    if DEBUG:
        print(f"[DEBUG] Translating node : {node}")

    # Cas d'une instruction de dessin
    if isinstance(node, tuple) and node[0] == 'draw':
        forme = node[1]
        parametres = node[2]

        if tabulation > 0 : 
            c_code += "\t" * tabulation

        c_code += f"draw_{forme}({', '.join(map(str, parametres))})"

        if semicolon :
            c_code += ";"

        if newline > 0 : 
            c_code += "\n" * newline
    
    # Cas d'un déplacement
    elif isinstance(node, tuple) and node[0] == 'move':
        x = node[1]
        y = node[2]

        if tabulation > 0 : 
            c_code += "\t" * tabulation

        c_code += f"move_to({x}, {y})"

        if semicolon :
            c_code += ";"

        if newline > 0 : 
            c_code += "\n" * newline
    
    # Cas d'une rotation
    elif isinstance(node, tuple) and node[0] == 'rotate':
        angle = node[1]

        if tabulation > 0 : 
            c_code += "\t" * tabulation

        c_code += f"rotate({angle})"

        if semicolon :
            c_code += ";"

        if newline > 0 : 
            c_code += "\n" * newline
    
    # Case: assignment
    elif isinstance(node, tuple) and node[0] == 'assign':
        var_name = node[1]
        value = node[2]

        if tabulation > 0:
            c_code += "\t" * tabulation

        if isinstance(value, str) and '"' in value:  # String or char case
            size = len(value) - 3  # Exclude surrounding quotes and initialize the index
            c_code += f"char {var_name}[{size}] = {value}"
        elif isinstance(value, str) and '"' not in value:  # Identifier case
            var_type, var_value = find_variable_in_ast(value, ast)
            if var_type and var_value is not None:
                c_code += f"{var_type} {var_name} = {value}"
            else:
                c_code += f"// Error: Variable '{value}' not found"
        else:  # Integer or float case
            c_code += f"{type(value).__name__} {var_name} = {value}"

        if semicolon:
            c_code += f"; // {value} = {var_value} and is {var_type}" if isinstance(value, str) and '"' not in value else ";"

        if newline > 0:
            c_code += "\n" * newline
    
    # Cas d'une couleur (assignation de couleur)
    elif isinstance(node, tuple) and node[0] == 'color':
        color = node[1]

        if tabulation > 0 : 
            c_code += "\t" * tabulation

        c_code += f"set_color({color})"

        if semicolon :
            c_code += ";"

        if newline > 0 : 
            c_code += "\n" * newline

    # Cas d'une conditionnelle (if)
    elif isinstance(node, tuple) and node[0] == 'if':
        condition = node[1]
        bloc_true = node[2][1] # renvoie la liste d'instructions du bloc (le programme)
        bloc_false = node[3][1] if len(node) == 4 else None # renvoie la liste d'instructions du bloc (le programme)

        if tabulation > 0 : 
            c_code += "\t" * tabulation

        c_code += f"if ({condition_to_c(condition)}) {{"
        
        for instr in bloc_true:
            c_code += f"\n{translate_node_to_c(ast,instr,1,tabulation+1,0)}"
        
        if tabulation > 0 : 
            c_code += "\t" * tabulation
        c_code += f"}}"

        if bloc_false :
            if tabulation > 0 : 
                c_code += "\t" * tabulation

            c_code += f"else {{"
            
            for instr in bloc_false:
                c_code += f"\n{translate_node_to_c(ast,instr,1,tabulation+1,0)}"
            
            if tabulation > 0 : 
                c_code += "\t" * tabulation
            c_code += f"}}"

        if newline > 0 : 
            c_code += "\n" * newline
                            
    # Cas d'une boucle (for)
    elif isinstance(node, tuple) and node[0] == 'for':
        init = node[1]
        condition = node[2]
        increment = node[3]
        bloc = node[4][1] # renvoie la liste d'instructions du bloc (le programme)

        if tabulation > 0 : 
            c_code += "\t" * tabulation

        c_code += f"for ({translate_node_to_c(ast,init,0,0,0)}; {condition_to_c(condition)}; {translate_node_to_c(ast,increment,0,0,0)}) {{\n"
        
        for instr in bloc:
            c_code += f"{translate_node_to_c(ast,instr,1,tabulation+1,0)}"

        if tabulation > 0 : 
            c_code += "\t" * tabulation
        c_code += f"}}"

        if newline > 0 : 
            c_code += "\n" * newline
    
    # Cas par défaut : on ne sait pas quel type est ce nœud, donc le code est ignoré
    else:
        c_code += f"// Node non pris en charge : {node}\n"
        return c_code

    print(f"[DEBUG] Translated succesfuly : {c_code}\n")

    return c_code

# === 2. Traduction de l'AST vers C ===
def translate_ast_to_c(ast):
    """Traduire l'AST en code C."""
    c_code = ""

    # Includes
    c_code += "#include <stdio.h>\n"
    c_code += "#include <stdlib.h>\n"
    c_code += "#include <math.h>\n\n"
    c_code += f"int main() {{\n\n"

    if DEBUG:
        print("\n[DEBUG] Translating AST to C code...")

    try:
        for node in ast:
            c_code += translate_node_to_c(ast,node,1,1,1)

    except Exception as e:
        print_error(f"Erreur pendant la traduction de l'AST : {e}")
        return None  # Signal an error occurred

    if DEBUG:
        print(f"[DEBUG] Successfully generated C code :")
        lines = c_code.splitlines()
        for i,line in enumerate(lines, start=1):
            print(f"[DEBUG] {i}\t{line}")

    c_code += f"\n\treturn 0;\n"
    c_code += f"}}\n"

    return c_code

# === 3. Exécution de l'AST ===
def execute_ast(ast, debug):
    """Exécute l'AST généré par le parser."""
    if not ast:
        print("Aucune instruction à exécuter.")
        return
    
    global DEBUG
    DEBUG = debug
    
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
    with open(filename, 'a') as f:
        f.write(c_code)