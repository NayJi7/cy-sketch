import os

# === 1. Gestion des erreurs ===
def print_error(error):
    """Affiche un message d'erreur formaté."""
    print(f"\033[31mErreur : {error}\033[0m")

def condition_to_c(condition):
    return f"{condition[1]} {condition[0]} {condition[2]}"

def resolve_value_and_type(ast, value):
    """
    Resolve the type and value of a variable or literal.
    - Handles literals, variables, and operations within the AST.
    - Recursively traverses nested structures (e.g., for, if, while).
    """
    if isinstance(value, (int, float)):  # Literal Numbers
        return type(value).__name__, value
    elif isinstance(value, str) and '"' in value:  # Literal String
        return "char*", value.strip('"')
    elif isinstance(value, str):  # Variable name
        # Search the AST for the variable declaration or assignment
        for node in ast:
            if isinstance(node, tuple):
                # Check direct assignments or modifications
                if node[0] in ('assign', 'modify') and node[1] == value:
                    resolved_type, resolved_value = resolve_value_and_type(ast, node[2])
                    return resolved_type, value
                # Handle sub-AST structures (e.g., for, while, if)
                elif node[0] == 'if':
                    sub_ast = node[2] if len(node) > 2 else []  # bloc
                    try:
                        # Recursively search in the sub-AST
                        return resolve_value_and_type(sub_ast, value)
                    except Exception:
                        continue  # If not found, continue searching other nodes
                elif node[0] == "for":
                    sub_ast = node[4] if len(node) > 2 else []  # bloc
                    try:
                        # Recursively search in the sub-AST
                        return resolve_value_and_type(sub_ast, value)
                    except Exception:
                        try:
                            return resolve_value_and_type([node[1]], value) # maybe the itterator
                        except Exception:
                            continue
        raise Exception(f"Variable '{value}' not initialized.")
    elif isinstance(value, tuple):  # Operation Node
        if value[0] == "op":
            # Resolve operation result type and expression
            left_type, left_c = resolve_value_and_type(ast, value[2])
            right_type, right_c = resolve_value_and_type(ast, value[3])
            operator = value[1]

            # Determine result type
            if left_type == "float" or right_type == "float":
                result_type = "float"
            else:
                result_type = "int"

            # Generate a flat expression
            expr = f"{left_c} {operator} {right_c}"
            return result_type, expr
        else:
            raise Exception(f"Unsupported operation type: {value[0]}")
    return None, None  # Unresolved

def find_variable_in_ast(var_name, ast_tree):
    """
    Search the AST for the declaration (initialize), assignment (assign), or modification of a given variable name.
    Returns the type and value of the variable.
    """
    for node in ast_tree:
        if isinstance(node, tuple):
            # Case 1: Assignment node
            if node[0] == 'assign':
                assigned_var_name = node[1]
                assigned_value = node[2]

                if assigned_var_name == var_name:
                    # Return the type and value of the variable in the assignment
                    if isinstance(assigned_value, str) and '"' in assigned_value:
                        size = len(assigned_value) - 2  # Removing quotes
                        return f"char[{size}]", assigned_value.strip('"')
                    elif isinstance(assigned_value, (int, float)):
                        return type(assigned_value).__name__, assigned_value
                    elif isinstance(assigned_value, str):  # Another identifier
                        return find_variable_in_ast(assigned_value, ast_tree)
                    elif isinstance(assigned_value, tuple) and assigned_value[0] == "op":
                        op = translate_node_to_c(ast_tree,assigned_value,0,0,0)
                        return op[0], op[1]

            # Case 2: Modify node (variable being modified)
            elif node[0] == 'modify':
                modified_var_name = node[1]
                new_value = node[2]

                if modified_var_name == var_name:
                    # Return the type and value of the modified variable
                    if isinstance(new_value, str) and '"' in new_value:
                        size = len(new_value) - 2  # Removing quotes
                        return f"char[{size}]", new_value.strip('"')
                    elif isinstance(new_value, (int, float)):
                        return type(new_value).__name__, new_value
                    elif isinstance(new_value, str):  # Another identifier
                        return find_variable_in_ast(new_value, ast_tree)

# === 2. Traduction des nodes vers C ===
def translate_node_to_c(ast, node, newline, tabulation, semicolon):
    """Traduire une node en code C."""
    c_code = ""

    if DEBUG:
        print(f"[DEBUG] Translating node : {node}")
    
    # Case: Drawing instruction (e.g., draw_circle)
    if isinstance(node, tuple) and node[0] == 'draw':
        forme = node[1]
        parametres = node[2]
        ok = True

        if tabulation > 0: 
            c_code += "\t" * tabulation

        if forme == "line":
            expected_args = 2
        elif forme == "circle":
            expected_args = 2
        elif forme == "arc":
            expected_args = 3
        elif forme == "square":
            expected_args = 4
        elif forme == "point":
            expected_args = 1

        for test in parametres:
            if type(test) == str and '"' not in test:
                t = find_variable_in_ast(test, ast)

                if t == None:
                    c_code += f"// ERROR : {test} variable not initialized"
                    ok = False

        if expected_args == len(parametres) and ok:
            c_code += f"draw_{forme}("
            for i, param in enumerate(parametres):
                c_code += f"{translate_node_to_c(ast, param, 0, 0, 0)}"
                if i<len(parametres) - 1:
                    c_code += ', '
            c_code += ')'
        elif expected_args != len(parametres) and ok :
            c_code += f"// ERROR : draw_{forme} function requires {expected_args} arguments, but you gave {len(parametres)}"            

        if semicolon:
            c_code += ";"

        if newline > 0:
            c_code += "\n" * newline
    
    # Case: Movement instruction (e.g., move_to)
    elif isinstance(node, tuple) and node[0] == 'move':
        x = node[1]
        y = node[2]
        ok = True

        if tabulation > 0: 
            c_code += "\t" * tabulation

        if type(x) != int or float:
            if type(x) == str and '"' not in x:
                t = find_variable_in_ast(x, ast)

                if t == None:
                    c_code += f"// ERROR : {x} variable not initialized"
                    ok = False
                elif t[0] != ("int" or "float"):
                    c_code += f"// ERROR : {x} is {t[0]}, expected int or float"
                    ok = False

        if type(y) != int or float:
            if type(y) == str and '"' not in y:
                t = find_variable_in_ast(y, ast)

                if t == None:
                    c_code += f"// ERROR : {y} variable not initialized"
                    ok = False
                elif t[0] != ("int" or "float"):
                    c_code += f"// ERROR : {y} is {t[0]}, expected int or float"
                    ok = False
            
        if ok:
            c_code += f"move_to({translate_node_to_c(ast,x,0,0,0)}, {translate_node_to_c(ast,y,0,0,0)})"

        if semicolon:
            c_code += ";"

        if newline > 0: 
            c_code += "\n" * newline
    
    # Case: Rotation instruction (e.g., rotate)
    elif isinstance(node, tuple) and node[0] == 'rotate':
        angle = node[1]
        ok = True

        if tabulation > 0: 
            c_code += "\t" * tabulation

        if type(angle) != int:
            if type(angle) == str and '"' not in angle:
                t = find_variable_in_ast(angle, ast)

                if t == None:
                    c_code += f"// ERROR : {angle} variable not initialized"
                    ok = False
                elif t[0] != "int":
                    c_code += f"// ERROR : {angle} is {t[0]}, expected int"
                    ok = False

        if ok:
            c_code += f"rotate({translate_node_to_c(ast, angle, 0,0,0)})"

        if semicolon:
            c_code += ";"

        if newline > 0: 
            c_code += "\n" * newline

    # Handle color assignment (e.g., set_color)
    elif isinstance(node, tuple) and node[0] == 'color':
        color = node[1]
        ok = True

        if tabulation > 0: 
            c_code += "\t" * tabulation

        if type(color) == str and '"' not in color:
            t = find_variable_in_ast(color, ast)

            if t == None:
                c_code += f"// ERROR : {color} variable not initialized"
                ok = False
            elif "char" not in t[0]:
                c_code += f"// ERROR : {color} is {t[0]}, expected str"
                ok = False
        
        if ok and type(color) != str: # security ++, normally it is stuck at the parser
            c_code += f"// ERROR : {color} is {type(color)}, expected str"
            ok = False

        if ok:
            c_code += f"set_color({color})"

        if semicolon:
            c_code += ";"

        if newline > 0: 
            c_code += "\n" * newline

    elif isinstance(node, int):
        return node
    
    elif isinstance(node, float):
        return node
    
    elif isinstance(node, str):
        return node
    
    # Case: Operation artihmetic
    elif isinstance(node, tuple) and node[0] == 'op':
        operator = node[1]

        # Generate code for the operation
        left_c = translate_node_to_c(ast, node[2], 0, 0, 0)
        right_c = translate_node_to_c(ast, node[3], 0, 0, 0)

        return f"{left_c} {operator} {right_c}"

    # Case: Modification (Assigning a value to a previously declared variable)
    elif isinstance(node, tuple) and node[0] == 'modify':
        var_name = node[1]
        value = node[2]
        var_type, var_val = resolve_value_and_type(ast,var_name)

        if tabulation > 0:
            c_code += "\t" * tabulation

        # Assign based on value type
        if isinstance(value, str) and '"' in value:  # String case
            # Allocate memory for the string in `a` (previously declared as void*)
            c_code += "\t" * tabulation + f'strcpy({var_name}, "{value.strip("\"")}")'  # Copy the string value
        elif isinstance(value, int):  # Integer case
            c_code += f'{var_name} = {value}'
        elif isinstance(value, float):  # Float case
            c_code += f'{var_name} = {value}'
        elif isinstance(value, str):  # Identifier case (a string variable)
            var_type, value = find_variable_in_ast(value, ast)
            if "char" in var_type:
                c_code += "\t" * tabulation + f'strcpy({var_name},{value})'  # Copy the string value
            elif var_type == ("int" or "float"):
                c_code += f'{var_name} = {value}'
            else:
                c_code += f"// Error: Unsupported type for variable '{value}'"
        # Case: Operation artihmetic
        elif isinstance(value, tuple) and value[0] == 'op':
            op_type, op = resolve_value_and_type(ast, value)
            if op_type != var_type:
                c_code += f"// Error: {var_name} is {var_type} and {op} returns {op_type}"
            else:
                c_code += f"{var_name} = {op}" # 0 = result type, 1 = string op 
        else:
            c_code += f"// Error: Unsupported value type"

        if semicolon:
            c_code += ";"  # Only one semicolon is added here

        if newline > 0:
            c_code += "\n" * newline

    # Case: Assignment with declaration
    elif isinstance(node, tuple) and node[0] == 'assign':
        var_name = node[1]
        value = node[2]

        if tabulation > 0:
            c_code += "\t" * tabulation

        # Assign based on type
        if isinstance(value, str) and '"' in value:  # String case
            size = len(value) - 2
            c_code += f'char[{size}] {var_name};\n'
            c_code += "\t" * tabulation + f'strcpy({var_name}, "{value.strip("\"")}")'
        elif isinstance(value, (int, float)):  # Number case
            c_code += f"{type(value).__name__} {var_name} = {value}"
        elif isinstance(value, str):  # Identifier
            var_type, var_value = find_variable_in_ast(value, ast)
            if var_type:
                c_code += f"{var_type} {var_name} = {value}"
            else:
                c_code += f"// Error: Variable '{value}' not found"
        # Case: Operation artihmetic
        elif isinstance(value, tuple) and value[0] == 'op':
            op_type, op = resolve_value_and_type(ast, value)
            c_code += f"{op_type} {var_name} = {op}"
        else:
            c_code += f"// Error: Unsupported value type"

        if semicolon:
            c_code += ";"

        if newline > 0:
            c_code += "\n" * newline

    # Handle conditional (if)
    elif isinstance(node, tuple) and node[0] == 'if':
        condition = node[1]
        bloc_true = node[2][1]  # List of instructions in the true block
        bloc_false = node[3][1] if len(node) == 4 else None  # False block if available

        if tabulation > 0: 
            c_code += "\t" * tabulation

        c_code += f"if ({condition_to_c(condition)}) {{"
        
        for instr in bloc_true:
            c_code += f"\n{translate_node_to_c(ast, instr, 1, tabulation+1, True)}"
        
        if tabulation > 0: 
            c_code += "\t" * tabulation
        c_code += f"}}"

        if bloc_false:
            if tabulation > 0: 
                c_code += "\t" * tabulation

            c_code += f"else {{"
            
            for instr in bloc_false:
                c_code += f"\n{translate_node_to_c(ast, instr, 1, tabulation+1, True)}"
            
            if tabulation > 0: 
                c_code += "\t" * tabulation
            c_code += f"}}"

        if newline > 0: 
            c_code += "\n" * (newline+1)

    # Handle loop (for)
    elif isinstance(node, tuple) and node[0] == 'for':
        init = node[1]
        condition = node[2]
        increment = node[3]
        bloc = node[4][1]  # List of instructions in the block

        if tabulation > 0: 
            c_code += "\t" * tabulation

        c_code += f"for ({translate_node_to_c(ast, init, 0, 0, 0)}; {condition_to_c(condition)}; {translate_node_to_c(ast, increment, 0, 0, 0)}) {{\n"
        
        for instr in bloc:
            c_code += f"{translate_node_to_c(ast, instr, 1, tabulation+1, True)}"

        if tabulation > 0: 
            c_code += "\t" * tabulation
        c_code += f"}}"

        if newline > 0: 
            c_code += "\n" * (newline+1)
    
    # Default case (unsupported node)
    else:
        c_code += f"// Node non pris en charge : {node}\n"
        return c_code

    print(f"[DEBUG] Translated successfully: {c_code}\n")

    return c_code

# === 2. Traduction de l'AST vers C ===
def translate_ast_to_c(ast):
    """Traduire l'AST en code C."""
    c_code = ""

    # Includes
    c_code += "#include <stdio.h>\n"
    c_code += "#include <stdlib.h>\n"
    c_code += "#include <string.h>\n"
    c_code += "#include <math.h>\n\n"

    c_code += "/////////////////////////////////////////////////////////////////////////////////////////////////////\n"
    c_code += "// This is a generated file. It is useless to edit it as it will be regenerated at each compilation//\n"
    c_code += "/////////////////////////////////////////////////////////////////////////////////////////////////////\n\n"

    c_code += f"int main() {{\n\n"

    if DEBUG:
        print("\n[DEBUG] Translating AST to C code...")

    try:
        for node in ast:
            c_code += translate_node_to_c(ast,node,1,1,1)

    except Exception as e:
        print_error(f"Erreur pendant la traduction de l'AST : {e}")
        return None  # Signal an error occurred

    c_code += f"\n\treturn 0;\n"
    c_code += f"}}\n"

    if DEBUG:
        print(f"[DEBUG] Successfully generated C code :")
        lines = c_code.splitlines()
        for i,line in enumerate(lines, start=1):
            print(f"[DEBUG] {i}\t{line}")


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