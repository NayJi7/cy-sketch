import os
import sys
import ply.yacc as yacc
from lexer import tokens, find_column, suggest_keyword

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
    '''instruction : dessin
                   | deplacement
                   | rotation
                   | couleur
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
def p_dessin(p):
    '''dessin : DRAW forme LPAREN parametres RPAREN'''
    p[0] = ('draw', p[2], p[4])
# @}

# @{
# @brief Specifies supported shapes for drawing.
def p_forme(p):
    '''forme : LINE
             | CIRCLE
             | SQUARE
             | ARC
             | POINT'''
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
           | expression_arithmetic'''
    p[0] = p[1]
# @}

def p_type(p):
    '''type : INT
            | FLOAT
            | CHAR'''
    p[0] = p[1]

def p_func_param(p):
    '''fparametres : fparametres COMMA type IDENTIFIER
                   | fparametres COMMA type IDENTIFIER LBRACKET NUMBER RBRACKET
                   | type IDENTIFIER
                   | type IDENTIFIER LBRACKET NUMBER RBRACKET'''
    if len(p) == 5:
        p[0] = p[1] + [p[3] + ' ' + p[4]]
    elif len(p) == 8:
        p[0] = p[1] + [p[3] + ' ' + str(p[4] + p[5] + str(p[6]) + p[7])]
    elif len(p) == 3:
        p[0] = [p[1] + ' ' + p[2]]
    elif len(p) == 6:
        p[0] = [p[1] + ' ' + str(p[2] + p[3] + str(p[4]) + p[5])]

# @{
# @brief Handles function declaration
def p_function(p):
    '''function : FUNC IDENTIFIER LPAREN fparametres RPAREN bloc
                | FUNC IDENTIFIER LPAREN RPAREN bloc'''
    p[0] = ('func', p[2], p[4], p[6]) if len(p) == 7 else ('func', p[2], p[5]) 
# @}

# @{
# @brief Handles function calls
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

# @{
# @brief Handles atomic arithmetic expressions like numbers, identifiers, or grouped expressions.
def p_expression_arithmetic_atomic(p):
    '''expression_arithmetic : LPAREN expression_arithmetic RPAREN
                             | NUMBER
                             | IDENTIFIER'''
    p[0] = p[1] if len(p) == 2 else p[2]
# @}

# @{
# @brief Handles movement instructions.
def p_deplacement(p):
    '''deplacement : MOVE LPAREN arg COMMA arg RPAREN'''
    p[0] = ('move', p[3], p[5])
# @}

# @{
# @brief Handles rotation instructions.
def p_rotation(p):
    '''rotation : ROTATE LPAREN arg RPAREN'''
    p[0] = ('rotate', p[3])
# @}

# @{
# @brief Handles color change instructions.
def p_couleur(p):
    '''couleur : COLOR LPAREN arg RPAREN'''
    p[0] = ('color', p[3])
# @}

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

# @{
# @brief Defines possible values: numbers, identifiers, strings, or booleans.
def p_valeur(p):
    '''valeur : NUMBER
              | IDENTIFIER
              | STRING
              | BOOLEAN'''
    p[0] = p[1]
# @}

# @{
# @brief Defines possible values: numbers, identifiers, strings, or booleans.
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
# @brief Handles conditional instructions with or without an else clause.
def p_conditionnelle(p):
    '''conditionnelle : IF LPAREN expression_logique RPAREN bloc ELSE bloc
                      | IF LPAREN expression_logique RPAREN bloc'''
    p[0] = ('if', p[3], p[5], p[7]) if len(p) == 8 else ('if', p[3], p[5])
# @}

# @{
# @brief Handles logical expressions with comparison operators or boolean values.
def p_expression_logique(p):
    '''expression_logique : valeur operateur_comparaison valeur
                          | valeur operateur_comparaison BOOLEAN
                          | BOOLEAN operateur_comparaison valeur
                          | BOOLEAN'''
    p[0] = (p[2], p[1], p[3]) if len(p) == 4 else p[1]
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
# @brief Handles loop instructions (while and for loops).
def p_boucle(p):
    '''boucle : WHILE LPAREN expression_logique RPAREN bloc
              | FOR LPAREN assignation SEMICOLON expression_logique SEMICOLON modification RPAREN bloc'''
    p[0] = ('while', p[3], p[5]) if len(p) == 6 else ('for', p[3], p[5], p[7], p[9])
# @}

# === 2. Syntax Error Handling ===

# @{
# @brief Handles syntax errors and provides suggestions based on the error type.
# List of valid keywords and tokens in your language
def p_error(p):
    if p:
        # Basic error message
        print(f"\033[31mSyntax error: '{p.value}' at line {p.lineno}, column {find_column(p.lexer.lexdata, p.lexpos)}\033[0m")
        
        # Provide specific suggestions based on token type
        if p.type in ("LPAREN", "RPAREN"):
            print("\033[34mSuggestion: Ensure parenthesis '(' and ')' are balanced.\033[0m")
        elif p.type in ("LBRACE", "RBRACE"):
            print("\033[34mSuggestion: Check that all braces '{' and '}' are properly matched.\033[0m")
        elif p.type == "SEMICOLON":
            print("\033[34mSuggestion: Remove the semicolons ';' at the end of each statement where it's not required.\033[0m")
        elif p.type == "IDENTIFIER":
            # Suggest corrections for identifiers
            suggestions = suggest_keyword(p.value)
            if suggestions:
                print(f"\033[34mDid you mean: {', '.join(suggestions)}?\033[0m")
            else:
                print("\033[34mSuggestion: Check if '{p.value}' is declared or is a valid keyword.\033[0m")
        elif p.type in ("NUMBER", "STRING", "BOOLEAN"):
            print("\033[34mSuggestion: Check the format of the value or verify its context in the statement.\033[0m")
        elif p.type in ("PLUS", "MINUS", "TIMES", "DIVIDE"):
            print("\033[34mSuggestion: Ensure the arithmetic operation has valid operands.\033[0m")
        elif p.type in ("EQ", "NEQ", "LT", "GT", "LE", "GE"):
            print("\033[34mSuggestion: Verify the comparison expression and its operands.\033[0m")
        elif p.type == "COMMA":
            print("\033[34mSuggestion: Check the use of commas in parameter lists or argument separation.\033[0m")
        else:
            print("\033[34mSuggestion: Verify syntax and refer to the language grammar.\033[0m")
    else:
        # Handle unexpected end of input
        print("\033[31mSyntax error: unexpected end of file.\033[0m")
        print("\033[34mSuggestion: Ensure all blocks, parentheses, or braces are properly closed.\033[0m")

    sys.exit(1)
# @}

# === 3. Parser Initialization ===

# @{
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
        debug=True,
        outputdir=generated_folder,
        tabmodule="parsetab"
    )

    return parser
# @}