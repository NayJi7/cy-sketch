import os
import sys
import copy
from lexer import suggest_keyword

# === 1. Error Handling ===

# @{
# @brief Displays a formatted error message.
# @param error The error message to be displayed.
def print_error(e):
    if type(e) is str:
        print(f"\033[31m{e}\033[0m")
        if "Unknown identifier" in e:
            suggestion = suggest_keyword(e.split("'")[1])
            if suggestion:
                print(f"\033[34mSuggestion : Did you mean '{suggestion}' ?\033[0m")
    sys.exit(1)
# @}

def print_error_interractive(e):
    if type(e) is str:
        print(f"\033[31m{e}\033[0m")
        if "Unknown identifier" in e:
            suggestion = suggest_keyword(e.split("'")[1])
            if suggestion:
                print(f"\033[34mSuggestion : Did you mean '{suggestion}' ?\033[0m")

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
        return f"char[{len(value.strip('"'))}]", value.strip('"')
    elif isinstance(value, str):  # Variable name
        # Search the AST for the variable assignment, or modification
        for i, node in enumerate(ast):
            if current_position is not None and i >= current_position:
                # If the current position is reached and variable is not found, return an error
                raise ValueError(f"ValueError : Variable '{value}' is used before initialization.")
            
            if isinstance(node, tuple):
                # function parameters
                if node[0] == 'func' and len(node) == 4:
                    params = node[2]  # Parameters of the function
                    list_types = ["int", "float", "char"]
                    # searching in parameters
                    for i, param in enumerate(params):
                        for tp in list_types:
                            param_without_type = param.replace (tp,"")
                            if value in param_without_type and param_without_type != param:
                                if "int" in param:
                                    return "int", None
                                elif "float" in param:
                                    return "float", None
                                elif "char" in param:
                                    return param.split()[0] + param.split()[1][param.split()[1].index('['):], None
                    # searching in the body of the function
                    sub_ast = copy.deepcopy(node[0])
                    try:
                        # Recursively search in the sub-AST
                        return resolve_value_and_find_variable(sub_ast, value, current_position)
                    except Exception:
                        continue  # If not found, continue searching other nodes

                elif node[0] == 'func' and len(node) == 3: # passed a modified node of func without bloc
                    params = node[2]  # Parameters of the function
                    list_types = ["int", "float", "char"]
                    # searching in parameters
                    for i, param in enumerate(params):
                        for tp in list_types:
                            param_without_type = param.replace (tp,"")
                            if value in param_without_type and param_without_type != param:
                                if "int" in param:
                                    return "int", None
                                elif "float" in param:
                                    return "float", None
                                elif "char" in param:
                                    return param.split()[0] + param.split()[1][param.split()[1].index('['):], None
                # Direct assignment or modification
                elif node[0] in ('assign', 'modify') and node[1] == value:
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
        raise ValueError(f"ValueError : Variable '{value}' is not initialized.")
    
    elif isinstance(value, tuple):  # Operation Node
        # A AMELIORER
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
            raise TypeError(f"TypeError : Unsupported operation type: {value[0]}")
    return None, None  # Unresolved
# @}

# === 3. AST Node Translation ===

def make_prototype(ast, node, prototypes):
    c_code = ""
    prototype= ""

    if DEBUG:
        print(f"[DEBUG] Writting the prototypes of the functions")

    if isinstance(node, tuple) and node[0] == 'func':
        name = node[1]
        params = node[2] if len(node) == 4 else []
        bloc = node[3][1] if len(node) == 4 else node[2][1]

        ret = None
        for instr in bloc:
            if isinstance(instr, tuple) and instr[0] == 'ret':
                ret = instr[1]
                break

        if ret == None:
            c_code += f"void {name}("
            prototype += f"void {name}("
        else:
            tp, value = resolve_value_and_find_variable(ast, ret)
            c_code += f"{tp.split('[')[0]+'*'} {name}(" if "char" in tp else f"{tp} {name}("
            prototype += f"{tp.split('[')[0]+'*'} {name}(" if "char" in tp else f"{tp} {name}("

        if params:
            for i, param in enumerate(params):
                    c_code += f"{translate_node_to_c(ast, prototypes, param, 0, 0, 0)}"
                    prototype += f"{translate_node_to_c(ast, prototypes, param, 0, 0, 0)}"
                    if i<len(params) - 1:
                        c_code += ', '
                        prototype += ', '
        c_code += f');\n'
        prototypes.append(prototype)

        return c_code

# @{
# @brief Translates a single AST node to C code.
# @param ast The abstract syntax tree containing program instructions.
# @param node The AST node to translate.
# @param newline Number of newlines to append after the code.
# @param tabulation Number of tabulations to prepend to the code.
# @param semicolon Whether to append a semicolon after the code.
# @param current_position The current position in the AST (default: None).
# @return A string containing the translated C code.
def translate_node_to_c(ast, prototypes, node, newline, tabulation, semicolon, current_position=None):
    """Traduire une node en code C."""
    c_code = ""

    if DEBUG:
        print(f"[DEBUG] Translating node : {node}")

    if isinstance(node, int):
        return node
    
    elif isinstance(node, float):
        return node
    
    elif isinstance(node, str):
        return node
    
    # Case: Drawing instruction (e.g., draw_circle)
    elif isinstance(node, tuple) and node[0] == 'draw':
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
                    raise ValueError(f"ValueError : '{test}' variable not initialized")

        if expected_args == len(parametres):
            c_code += f"draw_{forme}("
            for i, param in enumerate(parametres):
                c_code += f"{translate_node_to_c(ast, prototypes, param, 0, 0, 0)}"
                if i<len(parametres) - 1:
                    c_code += ', '
            c_code += ')'
        elif expected_args != len(parametres):
            raise IndexError(f"IndexError : draw_{forme} function requires {expected_args} arguments, but you gave {len(parametres)}")          

        if semicolon:
            c_code += ";"

        if newline > 0:
            c_code += "\n" * newline
    
    # Case: Movement instruction (e.g., move_to)
    elif isinstance(node, tuple) and node[0] == 'move':
        x = node[1]
        y = node[2]

        if tabulation > 0: 
            c_code += "\t" * tabulation

        if type(x) != int or float:
            if type(x) == str and '"' not in x:
                t = resolve_value_and_find_variable(ast, x, current_position)

                if t == None:
                    raise ValueError(f"ValueError : '{x}' variable not initialized")
                elif t[0] != ("int" or "float"):
                    raise TypeError(f"TypeError : '{x}' is {t[0]}, expected int or float")
            else:
                t = resolve_value_and_find_variable(ast, x, current_position)
                raise TypeError(f"TypeError : '{x}' is {t[0]}, expected int or float")

        if type(y) != int or float:
            if type(y) == str and '"' not in y:
                t = resolve_value_and_find_variable(ast, y, current_position)

                if t == None:
                    raise ValueError(f"ValueError : '{y}' variable not initialized")
                elif t[0] != ("int" or "float"):
                    raise TypeError(f"TypeError : '{y}' is {t[0]}, expected int or float")
            else:
                t = resolve_value_and_find_variable(ast, y, current_position)
                raise TypeError(f"TypeError : '{y}' is {t[0]}, expected int or float")
            
        c_code += f"move_to({translate_node_to_c(ast, prototypes,x,0,0,0)}, {translate_node_to_c(ast, prototypes,y,0,0,0)})"

        if semicolon:
            c_code += ";"

        if newline > 0: 
            c_code += "\n" * newline
    
    # Case: Rotation instruction (e.g., rotate)
    elif isinstance(node, tuple) and node[0] == 'rotate':
        angle = node[1]

        if tabulation > 0: 
            c_code += "\t" * tabulation

        if type(angle) != int:
            if type(angle) == str and '"' not in angle:
                t = resolve_value_and_find_variable(ast, angle, current_position)

                if t == None:
                    raise ValueError(f"ValueError : '{angle}' variable not initialized")
                elif t[0] != "int":
                    raise TypeError(f"TypeError : '{angle}' is {t[0]}, expected int")

        if ok:
            c_code += f"rotate({translate_node_to_c(ast, prototypes, angle, 0,0,0)})"

        if semicolon:
            c_code += ";"

        if newline > 0: 
            c_code += "\n" * newline

    # Handle color assignment (e.g., set_color)
    elif isinstance(node, tuple) and node[0] == 'color':
        color = node[1]

        if tabulation > 0: 
            c_code += "\t" * tabulation

        if type(color) == str and '"' not in color:
            t = resolve_value_and_find_variable(ast, color, current_position)

            if t == None:
                raise ValueError(f"ValueError : '{color}' variable not initialized")
            elif "char" not in t[0]:
                raise TypeError(f"TypeError : '{color}' is {t[0]}, expected char*")
        
        if type(color) != str: # security ++, normally it is detected in the parser
            raise TypeError(f"TypeError : '{color}' is {type(color)}, expected char*")

        c_code += f"set_color({color})"

        if semicolon:
            c_code += ";"

        if newline > 0: 
            c_code += "\n" * newline
    
    elif isinstance(node, tuple) and node[0] == 'func':
        name = node[1]
        params = node[2] if len(node) == 4 else []
        bloc = node[3][1] if len(node) == 4 else node[2][1]

        if tabulation > 0: 
            c_code += "\t" * tabulation

        ret = None
        for instr in bloc:
            if isinstance(instr, tuple) and instr[0] == 'ret':
                ret = instr[1]
                break

        if ret == None:
            c_code += f"void {name}("
        else:
            tp, value = resolve_value_and_find_variable(ast, ret, None)
            c_code += f"{tp.split('[')[0]+'*'} {name}(" if "char" in tp else f"{tp} {name}("

        if params:
            for i, param in enumerate(params):
                    c_code += f"{translate_node_to_c(ast, prototypes, param, 0, 0, 0)}"
                    if i<len(params) - 1:
                        c_code += ', '
        c_code += f'){{\n'

        for instr in bloc:
            c_code += f"{translate_node_to_c(ast, prototypes, instr, 1, tabulation+1, True)}"

        if tabulation > 0: 
            c_code += "\t" * tabulation

        c_code += f'}}'

        if newline > 0:
            c_code += "\n" * newline

    elif isinstance(node, tuple) and node[0] == 'func_call':
        name = node[1]
        params = node[2] if len(node) == 3 else []
        prototype = ""

        for prot in prototypes:
            if name in prot.split("(")[0]:
                prototype = prot
        
        if prototype == "" :
            raise ValueError(f"ValueError : '{name}' function not initialized")

        prot_params = prototype.split("(")[1][:-1] # On recup les parametres sans les parentèses
        prot_args = prot_params.split(',')

        for i, prot_arg in enumerate(prot_args):
            if "int" in prot_arg:
                prot_args[i] = "int"
            elif "float" in prot_arg:
                prot_args[i] = "float"
            elif "char" in prot_arg:
                prot_args[i] = f"char [{prot_arg.split("[")[1].split("]")[0]}]"

        for i, arg in enumerate(params):
            arg_type, arg_val = resolve_value_and_find_variable(ast, arg, current_position)
            
            if "char" in arg_type and "char" in prot_args[i]:
                if int(arg_type.split("[")[1].split("]")[0]) > int(prot_args[i].split("[")[1].split("]")[0]):
                    if i == 0:
                        raise IndexError(f"IndexError : '{arg_val}' is {int(arg_type.split("[")[1].split("]")[0])} long and {name} function's {i+1}st argument is not long enough ({prot_args[i].split("[")[1].split("]")[0]})")
                    elif i == 1:
                        raise IndexError(f"IndexError : '{arg_val}' is {int(arg_type.split("[")[1].split("]")[0])} long and {name} function's {i+1}nd argument is not long enough ({prot_args[i].split("[")[1].split("]")[0]})")
                    elif i == 2:
                        raise IndexError(f"IndexError : '{arg_val}' is {int(arg_type.split("[")[1].split("]")[0])} long and {name} function's {i+1}rd argument is not long enough ({prot_args[i].split("[")[1].split("]")[0]})")
                    else :
                        raise IndexError(f"IndexError : '{arg_val}' is {int(arg_type.split("[")[1].split("]")[0])} long and {name} function's {i+1}th argument is not long enough ({prot_args[i].split("[")[1].split("]")[0]})")
            elif arg_type not in prot_args[i]:
                if i == 0:
                    raise TypeError(f"TypeError : '{name}' function's {i+1}st argument is not {arg_type}. Expected : {prot_args[i].split()[0]}")
                elif i == 1:
                    raise TypeError(f"TypeError : '{name}' function's {i+1}nd argument is not {arg_type}. Expected : {prot_args[i].split()[0]}")
                elif i == 2:
                    raise TypeError(f"TypeError : '{name}' function's {i+1}rd argument is not {arg_type}. Expected : {prot_args[i].split()[0]}")
                else :
                    raise TypeError(f"TypeError : '{name}' function's {i+1}th argument is not {arg_type}. Expected : {prot_args[i].split()[0]}")
            
        if tabulation > 0: 
            c_code += "\t" * tabulation

        c_code += f"{name}("

        if params:
            for i, param in enumerate(params):
                c_code += f"{translate_node_to_c(ast, prototypes, param, 0, 0, 0)}"
                if i<len(params) - 1:
                    c_code += ', '
        c_code += f')'

        if semicolon:
            c_code += ";"

        if newline > 0:
            c_code += "\n" * newline

    elif isinstance(node, tuple) and node[0] == 'ret':
        value = node[1]

        c_code += "\n"

        if tabulation > 0: 
            c_code += "\t" * tabulation

        c_code += f"return {value}"

        if semicolon:
            c_code += ";"  # Only one semicolon is added here

        if newline > 0:
            c_code += "\n" * newline
    
    # Case: Operation artihmetic
    elif isinstance(node, tuple) and node[0] == 'op':
        operator = node[1]

        # Generate code for the operation
        left_c = translate_node_to_c(ast, prototypes, node[2], 0, 0, 0)
        right_c = translate_node_to_c(ast, prototypes, node[3], 0, 0, 0)

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
            if "char" not in var_type : raise TypeError(f"TypeError : '{var_name}' is {var_type} and your trying to assign a char* value") # Wrong type error
            # Allocate memory for the string in `a` (previously declared as void*)
            c_code += f'strcpy({var_name}, "{value.strip("\"")}")'  # Copy the string value
        elif isinstance(value, int):  # Integer case
            if var_type != "int" : raise TypeError(f"TypeError : '{var_name}' is {var_type} and your trying to assign a int value") # Wrong type error
            c_code += f'{var_name} = {value}'
        elif isinstance(value, float):  # Float case
            if var_type != "float" : raise TypeError(f"TypeError : '{var_name}' is {var_type} and your trying to assign a float value") # Wrong type error
            c_code += f'{var_name} = {value}'
        elif isinstance(value, str):  # Identifier case (a string variable)
            val_type, val = resolve_value_and_find_variable(ast, value, current_position)
            if "char" in val_type:
                if var_type != val_type : raise TypeError(f"TypeError : '{var_name}' is {var_type} and '{val}' is {val_type}") # Wrong type error
                c_code += f'strcpy({var_name},{val})' # Copy the string value
            elif val_type == ("int" or "float"):
                if var_type != val_type : raise TypeError(f"TypeError : '{var_name}' is {var_type} and '{val}' is {val_type}") # Wrong type error
                c_code += f'{var_name} = {val}'
            else:
                raise TypeError(f"TypeError: Unsupported type for variable '{value}'")
        # Case: Operation artihmetic
        elif isinstance(value, tuple) and value[0] == 'op':
            op_type, op = resolve_value_and_find_variable(ast, value, current_position)
            if op_type != var_type:
                raise TypeError(f"TypeError : '{var_name}' is {var_type} and {op} returns {op_type}")
            else:
                c_code += f"{var_name} = {op}" # 0 = result type, 1 = string op 
        else:
            raise TypeError(f"TypeError: Unsupported type for variable '{value}'")

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
            c_code += f'char {var_name}[{size}];\n'
            c_code += f'{"\t" * tabulation}strcpy({var_name}, "{value.strip("\"")}")'
        elif isinstance(value, (int, float)):  # Number case
            c_code += f"{type(value).__name__} {var_name} = {value}"
        elif isinstance(value, str):  # Identifier
            var_type, var_value = resolve_value_and_find_variable(ast, value, current_position)
            if "char" in var_type:
                c_code += f"{var_type.split('[')[0]} {var_name}{'['+var_type.split('[')[1]} = {value}"
            elif var_type:
                c_code += f"{var_type} {var_name} = {value}"
            else:
                raise ValueError(f"ValueError: Variable '{value}' not found")
        # Case: Operation artihmetic
        elif isinstance(value, tuple) and value[0] == 'op':
            op_type, op = resolve_value_and_find_variable(ast, value, current_position)
            c_code += f"{op_type} {var_name} = {op}"
        else:
            raise TypeError(f"TypeError: Unsupported value type for '{value}'")

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
            c_code += f"\n{translate_node_to_c(ast, prototypes, instr, 1, tabulation+1, True)}"
        
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
            c_code += f"\n{translate_node_to_c(ast, prototypes, instr, 1, tabulation+1, True)}"
        
        if tabulation > 0: 
            c_code += "\t" * tabulation
        c_code += f"}}"

        if bloc_false:
            if tabulation > 0: 
                c_code += "\t" * tabulation

            c_code += f"else {{"
            
            for instr in bloc_false:
                c_code += f"\n{translate_node_to_c(ast, prototypes, instr, 1, tabulation+1, True)}"
            
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

        c_code += f"for ({translate_node_to_c(ast, prototypes, init, 0, 0, 0)}; {condition_to_c(condition)}; {translate_node_to_c(ast, prototypes, increment, 0, 0, 0)}) {{\n"
        
        for instr in bloc:
            c_code += f"{translate_node_to_c(ast, prototypes, instr, 1, tabulation+1, True)}"

        if tabulation > 0: 
            c_code += "\t" * tabulation
        c_code += f"}}"

        if newline > 0: 
            c_code += "\n" * (newline+1)
    
    # Default case (unsupported node)
    else:
        raise Exception(f"CriticalError : Unsupported node -> {node}\n")

    if DEBUG:
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
    prototypes = []

    c_code += "/////////////////////////////////////////////////////////////////////////////////////////////////////\n"
    c_code += "// This is a generated file. It is useless to edit it as it will be regenerated at each compilation//\n"
    c_code += "/////////////////////////////////////////////////////////////////////////////////////////////////////\n\n"

    c_code += "//////////////////////////\n"
    c_code += "// Includes and Defines //\n"
    c_code += "//////////////////////////\n\n"

    # Includes
    c_code += "#include <stdio.h>\n"
    c_code += "#include <stdlib.h>\n"
    c_code += "#include <string.h>\n"
    c_code += "#include <math.h>\n\n"

    c_code += "#define TRUE 1\n"
    c_code += "#define FALSE 0\n\n"

    if DEBUG:
        print("\n[DEBUG] Translating AST to C code...")

    c_code += "///////////////////////////\n"
    c_code += "// Function's prototypes //\n"
    c_code += "///////////////////////////\n\n"

    try:
        for i, node in enumerate(ast):
            if isinstance(node, tuple) and node[0] == "func": # Gets the functions in the ast
                c_code += make_prototype(ast, node, prototypes) # Traduce the functions
        c_code += '\n'
    except Exception as e:
        print_error(f"Error during the making of the prototypes : {e}")
        return None  # Signal an error occurred

    c_code += "/////////////////////////////\n"
    c_code += "// Function's declarations //\n"
    c_code += "/////////////////////////////\n\n"

    topop = []
    try:
        for i, node in enumerate(ast):
            if isinstance(node, tuple) and node[0] == "func": # Gets the functions in the ast
                c_code += translate_node_to_c(ast, prototypes, node, 2, 0, 0, current_position=0) # Traduce the functions
                topop.append(i) # Save the node's position
    except Exception as e:
        print_error(f"Error during the traduction of the functions AST : {e}")
        return None  # Signal an error occurred
    
    if topop:
        for i,nodeidx in enumerate(topop): # we enumerate the list to parse and pop the function's nodes that has been translated
            ast.pop(nodeidx - i) # - i because when you pop a node every index goes - 1. We keep the track using i

    c_code += "///////////////////\n"
    c_code += "// Main function //\n"
    c_code += "///////////////////\n\n"

    c_code += f"int main() {{\n\n"

    try:
        for i, node in enumerate(ast):
            c_code += translate_node_to_c(ast, prototypes, node, 1, 1, 1, current_position=i)
    except Exception as e:
        print_error(f"Error during the traduction of the main AST : {e}")
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

    print(f"\033[32m\nGenerated C code successfully. Written to: {filename}[0m\033")
# @}