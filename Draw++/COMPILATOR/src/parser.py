import os
import sys
import ply.yacc as yacc
from COMPILATOR.src.lexer import tokens, find_column, suggest_keyword

# @brief Parsing module for Draw++ language
# @details Defines grammar and production rules for the syntactic analyzer

# === 1. Parser Rules ===

# @{
# @brief Defines the structure of a program.
# @details A program is composed of one or more instructions.
def p_programme(p):
    '''programme : instruction
                 | programme instruction'''
    p[0] = [p[1]] if len(p) == 2 else p[1] + [p[2]]
# @}

# @{
# @brief Defines different types of instructions in the program.
def p_instruction(p):
    '''instruction : draw
                   | set
                   | assignation
                   | modification
                   | conditionnelle
                   | boucle
                   | bloc
                   | function
                   | func
                   | return'''
    p[0] = p[1]
# @}

# @{
# @brief Handles draw instructions.
# @param p A tuple containing the draw command, shape, and parameters.
def p_draw(p):
    '''draw : DRAW forme LPAREN parametres RPAREN'''
    p[0] = ('draw', p[2], p[4], p.lineno(1))
# @}

# @{
# @brief Specifies supported shapes for drawing.
def p_forme(p):
    '''forme : LINE
             | CIRCLE
             | ELLIPSE
             | POLYGON
             | SQUARE
             | TRIANGLE
             | RECTANGLE
             | ARC'''
    p[0] = p[1]
# @}

# @{
# @brief Defines a list of parameters for a command.
def p_parametres(p):
    '''parametres : parametres COMMA arg
                  | arg'''
    p[0] = p[1] + [p[3]] if len(p) == 4 else [p[1]]
# @}

# @{
# @brief Defines an argument, which can be a number, identifier, string, or boolean.
def p_arg(p):
    '''arg : NUMBER
           | IDENTIFIER
           | STRING
           | BOOLEAN
           | color
           | specialparam
           | expression_arithmetic'''
    p[0] = p[1]
# @}

# @brief Defines special parameters for shapes
# @param p Tuple containing production information
def p_specialparam(p):
    '''specialparam : ANIMATED
                    | INSTANT
                    | FILLED
                    | EMPTY'''
    p[0] = p[1]

# @brief Defines available colors
# @param p Tuple containing production information
def p_color(p):
    '''color : RED
             | GREEN
             | BLUE
             | WHITE
             | BLACK
             | YELLOW
             | CYAN
             | MAGENTA
             | GRAY
             | LIGHT_GRAY
             | DARK_GRAY
             | ORANGE
             | PURPLE
             | BROWN
             | PINK'''
    p[0] = p[1]

# @brief Defines basic data types
# @param p Tuple containing production information
def p_type(p):
    '''type : INT
            | FLOAT
            | CHAR'''
    p[0] = p[1]

# @brief Handles function parameters
# @param p Tuple containing production information
# @details Supports simple parameters and arrays
def p_func_param(p):
    '''fparametres : fparametres COMMA type IDENTIFIER
                   | fparametres COMMA type IDENTIFIER LBRACKET NUMBER RBRACKET
                   | type IDENTIFIER
                   | type IDENTIFIER LBRACKET NUMBER RBRACKET'''
    if len(p) == 5:         # Regular parameter with type
        p[0] = p[1] + [p[3] + ' ' + p[4]]
    elif len(p) == 8:       # Array parameter with size
        p[0] = p[1] + [p[3] + ' ' + str(p[4] + p[5] + str(p[6]) + p[7])]
    elif len(p) == 3:       # Single parameter with type
        p[0] = [p[1] + ' ' + p[2]]
    elif len(p) == 6:       # Single array parameter with size
        p[0] = [p[1] + ' ' + str(p[2] + p[3] + str(p[4]) + p[5])]

# @{
# @brief Handles function declaration
def p_function(p):
    '''function : FUNC IDENTIFIER LPAREN fparametres RPAREN bloc
                | FUNC IDENTIFIER LPAREN RPAREN bloc'''
    p[0] = ('func', p[2], p[4], p[6]) if len(p) == 7 else ('func', p[2], p[5]) 
# @}

# @brief Gère les appels de fonction
# @param p Tuple contenant les informations de production
def p_func(p):
    '''func : IDENTIFIER LPAREN parametres RPAREN
            | IDENTIFIER LPAREN RPAREN'''
    p[0] = ('func_call', p[1], p[3]) if len(p) == 5 else ('func_call', p[1]) 
# @}

# @{
# @brief Handles arithmetic expressions with binary operators.
def p_expression_arithmetic(p):
    '''expression_arithmetic : expression_arithmetic PLUS expression_arithmetic
                             | expression_arithmetic MINUS expression_arithmetic
                             | expression_arithmetic TIMES expression_arithmetic
                             | expression_arithmetic DIVIDE expression_arithmetic'''
    p[0] = ('op', p[2], p[1], p[3])
# @}

# @brief Defines rules for atomic arithmetic expressions
# @param p Tuple containing production information
def p_expression_arithmetic_atomic(p):
    '''expression_arithmetic : LPAREN expression_arithmetic RPAREN
                             | NUMBER
                             | IDENTIFIER'''
    p[0] = p[1] if len(p) == 2 else p[2]
# @}

# @brief Handles color definition instructions
# @param p Tuple containing production information
def p_set_color(p):
    '''set : SET elem COLOR LPAREN color RPAREN'''
    p[0] = ('setcolor', p[2], p[5])

# @brief Handles window size definition instructions
# @param p Tuple containing production information
def p_set_size_win(p):
    '''set : SET WINDOW SIZE LPAREN NUMBER COMMA NUMBER RPAREN'''
    p[0] = ('setsize', p[2], p[5], p[7])

# @brief Handles cursor size definition instructions
# @param p Tuple containing production information
def p_set_size_curs(p):
    '''set : SET CURSOR SIZE LPAREN NUMBER RPAREN'''
    p[0] = ('setsize', p[2], p[5])

# @brief Defines modifiable elements (window, cursor)
# @param p Tuple containing production information
def p_elem(p):
    '''elem : WINDOW
            | CURSOR'''
    p[0] = p[1]

# @{
# @brief Handles variable assignments.
def p_assignation(p):
    '''assignation : VARIABLE IDENTIFIER EQUALS arg'''
    p[0] = ('assign', p[2], p[4])
# @}

# @{
# @brief Handles modifications of existing variables.
def p_modification(p):
    '''modification : IDENTIFIER EQUALS arg'''
    p[0] = ('modify', p[1], p[3])
# @}

# @brief Defines possible values (numbers, identifiers, strings, booleans)
# @param p Tuple containing production information
def p_valeur(p):
    '''valeur : NUMBER
              | IDENTIFIER
              | STRING
              | BOOLEAN'''
    p[0] = p[1]
# @}

# @brief Handles return statement
# @param p Tuple containing production information
def p_return(p):
    '''return : RET valeur'''
    p[0] = ('ret', p[2])
# @}

# @{
# @brief Handles blocks of instructions enclosed in braces.
def p_bloc(p):
    '''bloc : LBRACE programme RBRACE'''
    p[0] = ('block', p[2])
# @}

# @{
# @brief Handles conditional instructions with elif and optional else clause.
def p_conditionnelle(p):
    '''conditionnelle : IF LPAREN expression_logique RPAREN bloc elif_clauses ELSE bloc
                      | IF LPAREN expression_logique RPAREN bloc elif_clauses
                      | IF LPAREN expression_logique RPAREN bloc ELSE bloc
                      | IF LPAREN expression_logique RPAREN bloc'''
    if len(p) == 9:  # If with elif and else
        p[0] = ('if', p[3], p[5], p[6], p[8])
    elif len(p) == 7:  # If with elif
        p[0] = ('if', p[3], p[5], p[6])
    elif len(p) == 8:  # If with else
        p[0] = ('if', p[3], p[5], p[7])
    else:  # Simple if
        p[0] = ('if', p[3], p[5])

# @brief Handles elif clauses.
def p_elif_clauses(p):
    '''elif_clauses : ELIF LPAREN expression_logique RPAREN bloc elif_clauses
                    | ELIF LPAREN expression_logique RPAREN bloc'''
    if len(p) == 7:  # Multiple elifs
        p[0] = [('elif', p[3], p[5])] + p[6]
    else:  # Single elif
        p[0] = [('elif', p[3], p[5])]
# @}

# @{
# @brief Handles logical expressions with comparison operators, boolean values, and logical operators.
def p_expression_logique(p):
    '''expression_logique : valeur operateur_comparaison valeur
                          | valeur operateur_comparaison BOOLEAN
                          | BOOLEAN operateur_comparaison valeur
                          | BOOLEAN
                          | expression_logique AND expression_logique
                          | expression_logique OR expression_logique'''
    if len(p) == 4 and p[2] in ('AND', 'OR'):
        p[0] = (p[2], p[1], p[3])  # Logical operators
    elif len(p) == 4:
        p[0] = (p[2], p[1], p[3])  # Comparison operators
    else:
        p[0] = p[1]                # Single boolean or value
# @}

# @{
# @brief Defines supported comparison operators.
def p_operateur_comparaison(p):
    '''operateur_comparaison : EQ
                             | NEQ
                             | LT
                             | GT
                             | LE
                             | GE'''
    p[0] = p[1]
# @}

# @{
# @brief Handles loop instructions (while, for, and do-while loops).
def p_boucle(p):
    '''boucle : WHILE LPAREN expression_logique RPAREN bloc
              | FOR LPAREN assignation SEMICOLON expression_logique SEMICOLON modification RPAREN bloc
              | DO bloc WHILE LPAREN expression_logique RPAREN'''
    if len(p) == 6:  # WHILE loop
        p[0] = ('while', p[3], p[5])
    elif len(p) == 10:  # FOR loop
        p[0] = ('for', p[3], p[5], p[7], p[9])
    elif len(p) == 7:  # DO-WHILE loop
        p[0] = ('dowhile', p[2], p[5])
# @}

# === 2. Syntax Error Handling ===

# @{
# @brief Handles syntax errors and provides suggestions based on the error type.
def p_error(p):
    if p:
        # Basic error message with line and column info
        error_msg = f"Syntax error: '{p.value}' at line {p.lineno}, column {find_column(p.lexer.lexdata, p.lexpos)}"
        
        # Provide context-specific suggestions
        if p.type in ("LPAREN", "RPAREN"):
            suggestion = "Ensure parenthesis '(' and ')' are balanced."
        elif p.type in ("LBRACE", "RBRACE"):
            suggestion = "Check that all braces '{' and '}' are properly matched."
        elif p.type == "SEMICOLON":
            suggestion = "Remove the semicolons ';' at the end of each statement where it's not required."
        elif p.type == "IDENTIFIER":
            # Suggest corrections for identifiers
            suggestions = suggest_keyword(p.value)
            if suggestions:
                suggestion = f"Did you mean '{suggestions}'?"
            else:
                suggestion = f"Check if '{p.value}' is declared or is a valid keyword."
        elif p.type in ("NUMBER", "STRING", "BOOLEAN"):
            suggestion = "Check the format of the value or verify its context in the statement."
        elif p.type in ("PLUS", "MINUS", "TIMES", "DIVIDE"):
            suggestion = "Ensure the arithmetic operation has valid operands."
        elif p.type in ("EQ", "NEQ", "LT", "GT", "LE", "GE"):
            suggestion = "Verify the comparison expression and its operands."
        elif p.type == "COMMA":
            suggestion = "Check the use of commas in parameter lists or argument separation."
        else:
            suggestion = "Verify syntax and refer to the language grammar."

        print(f"-#blue Suggestion: {suggestion}")
        
        # Raise exception for the IDE's error highlighting
        raise SyntaxError(f"{error_msg}")
    else:
        # Handle unexpected end of input
        error_msg = "Syntax error: unexpected end of file."
        suggestion = "Ensure all blocks, parentheses, or braces are properly closed."
        print(f"-#blue Suggestion: {suggestion}")
        raise SyntaxError(error_msg)
# @}

# === 3. Parser Initialization ===

# @brief Initializes the parser and generates parsing tables in a specified directory.
def init_parser():
    # Directory for generated parser files
    generated_folder = os.path.join(os.path.dirname(__file__), '.parser_generated')

    # Create the directory if it doesn't exist
    if not os.path.exists(generated_folder):
        os.makedirs(generated_folder)

    # Add the generated folder to the system path
    if generated_folder not in sys.path:
        sys.path.insert(0, generated_folder)

    # Build the parser with generated files in the specified directory
    parser = yacc.yacc(
        debug=False,
        outputdir=generated_folder,
        tabmodule="parsetab"
    )

    return parser
# @}