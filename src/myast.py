import os

# === 1. Error Handling ===

# @{
# @brief Displays a formatted error message.
# @param error The error message to be displayed.
def print_error(error):
    print(f"\033[31mErreur : {error}\033[0m")
# @}

# @{
# @brief Converts a condition to its C representation.
# @param condition The condition to convert (boolean or tuple).
# @return A string representing the condition in C syntax.
def condition_to_c(condition):
    if type(condition) == bool:
        return 'TRUE' if condition else 'FALSE'
    elif condition[1] == (True or False):
        return f"TRUE {condition[0]} {condition[2]}" if condition[1] else f"FALSE {condition[0]} {condition[2]}"
    elif condition[2] == (True or False):
        return f"{condition[1]} {condition[0]} TRUE" if condition[2] else f"{condition[1]} {condition[0]} FALSE"
    else:
        return f"{condition[1]} {condition[0]} {condition[2]}"
# @}

# === 2. AST Node Resolution ===

# @{
# @brief Resolves the type and value of a variable or literal.
# @param ast The abstract syntax tree containing program instructions.
# @param value The value or variable to resolve.
# @param current_position The current position in the AST (default: None).
# @return A tuple containing the type and value of the resolved variable or literal.
def resolve_value_and_find_variable(ast, value, current_position=None):
    """
    Resolve the type and value of a variable or literal.
    - Ensures the variable is initialized before the current position.
    - Handles literals, variables, operations, and nested AST structures.
    """
    if isinstance(value, (int, float)):  # Literal Numbers
        return type(value).__name__, value
    elif isinstance(value, str) and '"' in value:  # Literal String
        return "char*", value.strip('"')
    elif isinstance(value, str):  # Variable name
        # Search the AST for the variable declaration, assignment, or modification
        for i, node in enumerate(ast):
            if current_position is not None and i >= current_position:
                # If the current position is reached and variable is not found, raise an error
                raise Exception(f"Variable '{value}' is used before initialization.")

            if isinstance(node, tuple):
                # Direct assignment or modification
                if node[0] in ('assign', 'modify') and node[1] == value:
                    return resolve_value_and_find_variable(ast, node[2])[0], value
                # Handle nested structures (if, for, while)
                elif node[0] == 'if':
                    sub_ast = node[2] if len(node) > 2 else []  # Block
                    try:
                        # Recursively search in the sub-AST
                        return resolve_value_and_find_variable(sub_ast, value, None)
                    except Exception:
                        continue  # If not found, continue searching other nodes
                elif node[0] == "for":
                    sub_ast = node[4] if len(node) > 2 else []  # Block
                    try:
                        # Recursively search in the sub-AST
                        return resolve_value_and_find_variable(sub_ast, value, None)
                    except Exception:
                        try:
                            return resolve_value_and_find_variable([node[1]], value)  # Maybe the iterator
                        except Exception:
                            continue
        # If the variable is not found in the AST
        raise Exception(f"Variable '{value}' is not initialized.")
    elif isinstance(value, tuple):  # Operation Node
        if value[0] == "op":
            # Resolve operation result type and expression
            left_type, left_c = resolve_value_and_find_variable(ast, value[2], current_position)
            right_type, right_c = resolve_value_and_find_variable(ast, value[3], current_position)
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
# @}

# === 3. AST Node Translation ===

# @{
# @brief Translates a single AST node to C code.
# @param ast The abstract syntax tree containing program instructions.
# @param node The AST node to translate.
# @param newline Number of newlines to append after the code.
# @param tabulation Number of tabulations to prepend to the code.
# @param semicolon Whether to append a semicolon after the code.
# @param current_position The current position in the AST (default: None).
# @return A string containing the translated C code.
def translate_node_to_c(ast, node, newline, tabulation, semicolon, current_position=None):
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
                t = resolve_value_and_find_variable(ast, test, current_position)

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
                t = resolve_value_and_find_variable(ast, x, current_position)

                if t == None:
                    c_code += f"// ERROR : {x} variable not initialized"
                    ok = False
                elif t[0] != ("int" or "float"):
                    c_code += f"// ERROR : {x} is {t[0]}, expected int or float"
                    ok = False

        if type(y) != int or float:
            if type(y) == str and '"' not in y:
                t = resolve_value_and_find_variable(ast, y, current_position)

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
                t = resolve_value_and_find_variable(ast, angle, current_position)

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
            t = resolve_value_and_find_variable(ast, color, current_position)

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
        var_type, var_val = resolve_value_and_find_variable(ast,var_name, current_position)

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
            var_type, value = resolve_value_and_find_variable(ast, value, current_position)
            if "char" in var_type:
                c_code += "\t" * tabulation + f'strcpy({var_name},{value})'  # Copy the string value
            elif var_type == ("int" or "float"):
                c_code += f'{var_name} = {value}'
            else:
                c_code += f"// Error: Unsupported type for variable '{value}'"
        # Case: Operation artihmetic
        elif isinstance(value, tuple) and value[0] == 'op':
            op_type, op = resolve_value_and_find_variable(ast, value, current_position)
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
            var_type, var_value = resolve_value_and_find_variable(ast, value, current_position)
            if var_type:
                c_code += f"{var_type} {var_name} = {value}"
            else:
                c_code += f"// Error: Variable '{value}' not found"
        # Case: Operation artihmetic
        elif isinstance(value, tuple) and value[0] == 'op':
            op_type, op = resolve_value_and_find_variable(ast, value, current_position)
            c_code += f"{op_type} {var_name} = {op}"
        else:
            c_code += f"// Error: Unsupported value type"

        if semicolon:
            c_code += ";"

        if newline > 0:
            c_code += "\n" * newline

    elif isinstance(node, tuple) and node[0] == "while":
        condition = node[1]
        bloc = node[2][1]

        if tabulation > 0: 
            c_code += "\t" * tabulation

        c_code += f"while ({condition_to_c(condition)}) {{"
        
        for instr in bloc:
            c_code += f"\n{translate_node_to_c(ast, instr, 1, tabulation+1, True)}"
        
        if tabulation > 0: 
            c_code += "\t" * tabulation
        c_code += f"}}"

        if newline > 0: 
            c_code += "\n" * (newline+1)

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
# @}

# === 4. AST to C Code Translation ===

# @{
# @brief Translates the entire AST into a C program.
# @param ast The abstract syntax tree containing program instructions.
# @return A string containing the translated C program.
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
        for i, node in enumerate(ast):
            c_code += translate_node_to_c(ast, node, 1, 1, 1, current_position=i)

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
# @}

# === 5. AST Execution ===

# @{
# @brief Executes the AST by translating it to C and writing it to a file.
# @param ast The abstract syntax tree containing program instructions.
# @param debug A flag to enable or disable debug output.
def execute_ast(ast, debug):
    """Exécute l'AST généré par le parser."""
    if not ast:
        print("Aucune instruction à exécuter.")
        return
    
    global DEBUG
    DEBUG = debug
    
    # Traduction de l'AST en code C
    c_code = translate_ast_to_c(ast)

    # Determine the directory of the current script
    script_dir = os.path.dirname(os.path.abspath(__file__))

    # Set the file path for generated_code.c
    filename = os.path.join(script_dir, "../.to_compil.c")
    
    # Write the generated C code to the file
    with open(filename, 'w') as f:
        f.write(c_code)

    print(f"\n[DEBUG] Generated C code written to: {filename}")
# @}