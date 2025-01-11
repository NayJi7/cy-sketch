import re
import ply.lex as lex
from difflib import get_close_matches

# @brief Suggests a similar keyword for a given word
# @param word The word to find suggestions for
# @return The closest keyword match or None if no match found
def suggest_keyword(word):
    suggestions = get_close_matches(word, keywords.keys(), n=1, cutoff=0.6)
    return suggestions[0] if suggestions else None

# @brief Retrieves all declared identifiers in the code
# @param t Token containing lexer information
# @return List of known identifiers
def get_known_identifiers(t):
    known_identifiers = []

    text = t.lexer.lexdata
    lines = text.split('\n')
    lines = [x for x in lines if x != ""]  # Remove empty lines
    for line in lines:
        line = line.strip('\n')  # Remove trailing newlines
        line = line.strip('\t')  # Remove leading/trailing tabs

        # Match variable declarations
        resultat = re.search(r'var (\w+)\s*=', line)
        if resultat:
            known_identifiers.append(resultat.group(1))

        # Match function declarations
        resultat = re.search(r'func\s+(\w+)\s*\(', line)
        if resultat:
            known_identifiers.append(resultat.group(1))

        # Match integer variable declarations
        resultat = re.search(r'int\s+(\w+)[\s,)]', line)
        if resultat:
            known_identifiers.append(resultat.group(1))

        # Match float variable declarations
        resultat = re.search(r'float\s+(\w+)[\s,)]', line)
        if resultat:
            known_identifiers.append(resultat.group(1))

        # Match char variable declarations
        resultat = re.search(r'char\s+(\w+)', line)
        if resultat:
            known_identifiers.append(resultat.group(1))

    return known_identifiers

# === 1. Definition of Tokens ===

# @{
# @brief Dictionary of available colors
colors = {
    "red": "red",
    "green": "green",
    "blue": "blue",
    "white": "white",
    "black": "black",
    "yellow": "yellow",
    "cyan": "cyan",
    "magenta": "magenta",
    "gray": "gray",
    "light_gray": "light_gray",
    "dark_gray": "dark_gray",
    "orange": "orange",
    "purple": "purple",
    "brown": "brown",
    "pink": "pink",
}

# @{
# @brief Reserved keywords and their corresponding token names
keywords = {
    'if': 'IF',
    'elif': 'ELIF',
    'else': 'ELSE',
    'for': 'FOR',
    'while': 'WHILE',
    'func': 'FUNC',
    'int' : 'INT',
    'float' : 'FLOAT',
    'char' : 'CHAR',
    'return': 'RET',
    'draw': 'DRAW',
    'cursor': 'CURSOR',
    'window': 'WINDOW',
    'color': 'COLOR',
    'size': 'SIZE',
    'set': 'SET',
    'animated': 'ANIMATED',
    'instant': 'INSTANT',
    'empty': 'EMPTY',
    'filled': 'FILLED',
    'var': 'VARIABLE',
    'ellipse': 'ELLIPSE',
    'polygon': 'POLYGON',
    'line': 'LINE',
    'circle': 'CIRCLE',
    'triangle': 'TRIANGLE',
    'square': 'SQUARE',
    'rectangle': 'RECTANGLE',
    'arc': 'ARC',
    'or': 'OR',
}

# Add colors to keywords
keywords.update(colors)  # Use update instead of | operator for Python 3.6 compatibility

# @}

# @{
# @brief List of all token names, including operators, separators, and keywords
tokens = [
    'IDENTIFIER', 'NUMBER', 'STRING', 'BOOLEAN',
    'PLUS', 'MINUS', 'TIMES', 'DIVIDE', 'EQUALS',
    'LT', 'GT', 'LE', 'GE', 'EQ', 'NEQ', 'AND',
    'LPAREN', 'RPAREN', 'LBRACE', 'RBRACE', 'LBRACKET', 
    'RBRACKET','COMMA', 'SEMICOLON',
] + list(keywords.values())
# @}

# === 2. Token Rules ===

# @{
# @brief Recognition rules for comparison and arithmetic operators
t_PLUS = r'\+'
t_MINUS = r'-'
t_TIMES = r'\*'
t_DIVIDE = r'/'
t_EQUALS = r'='
t_LT = r'<'
t_GT = r'>'
t_LE = r'<='
t_GE = r'>='
t_EQ = r'=='
t_NEQ = r'!='
# @}

# @{
# @brief Recognition rules for delimiters
t_LPAREN = r'\('
t_RPAREN = r'\)'
t_LBRACE = r'\{'
t_LBRACKET = r'\['
t_RBRACKET = r'\]'
t_COMMA = r','
# @}

# @brief Recognition rule for closing brace
# @param t Token containing the matched value
# @return Processed token
def t_RBRACE(t):
    r'\}'
    return t

# @brief Recognition rule for semicolon
# @param t Token containing the matched value
# @return Processed token
def t_SEMICOLON(t):
    r';'
    return t

# @brief Matches identifiers and checks if they correspond to reserved keywords
# @param t Token object containing the matched value
# @return Token object with the correct type
def t_IDENTIFIER(t):
    r'[a-zA-Z_][a-zA-Z0-9_]*'

    known_identifiers = get_known_identifiers(t)

    if t.value in keywords:
        t.type = keywords[t.value]
    elif t.value in ['true', 'false']:  # Handle boolean literals
        t.type = 'BOOLEAN'
        t.value = (t.value == 'true')
    elif t.value in known_identifiers:
        t.type = 'IDENTIFIER'
    else:
        error_msg = f"Unknown identifier '{t.value}' at line {t.lexer.lineno}, column {find_column(t.lexer.lexdata, t.lexpos)}"
        raise SyntaxError(error_msg)
    return t

# @brief Matches numbers, both integers and floats
# @param t Token object containing the matched value
# @return Token object with the number converted to int or float
def t_NUMBER(t):
    r'\d+(\.\d+)?'
    t.value = float(t.value) if '.' in t.value else int(t.value)
    return t

# @brief Matches strings enclosed in double quotes
# @param t Token object containing the matched value
# @return Token object with the string value
def t_STRING(t):
    r'("(?:[^"\\]|\\.)*")'
    return t

# @brief Matches boolean literals (True or False)
# @param t Token object containing the matched value
# @return Token object with boolean value
def t_BOOLEAN(t):
    r'true|false'
    t.value = (t.value == 'true')
    return t

# @brief Ignores spaces and tabs
t_ignore = ' \t'

# @brief Recognition rule for newlines
# @param t Token containing the matched value
def t_newline(t):
    r'\n+'
    t.lexer.lineno += len(t.value)

# @brief Recognition rule for single-line comments
# @param t Token containing the matched value
def t_COMMENT(t):
    r'\#.*'
    t.lexer.lineno += t.value.count('\n')
    pass

# @brief Recognition rule for multi-line comments
# @param t Token containing the matched value
def t_COMMENT_MULTILINE(t):
    r'/\*([^*]|\*[^/])*\*/'
    t.lexer.lineno += t.value.count('\n')
    pass

# @brief Handles lexical errors by raising an exception
# @param t Token object containing the unmatched value
def t_error(t):
    error_msg = f"Illegal character '{t.value[0]}' at line {t.lineno}, column {find_column(t.lexer.lexdata, t.lexpos)}"
    raise SyntaxError(error_msg)

# @brief Finds the column number of a given position in the input text
# @param input_text The full input string
# @param lexpos The position of the error in the input text
# @return The column number of the error
def find_column(input_text, lexpos):
    last_newline = input_text.rfind('\n', 0, lexpos)
    return (lexpos - last_newline) if last_newline >= 0 else lexpos + 1

# @brief Initializes the lexer using PLY
# @return A PLY lexer object
def init_lexer():
    lexer = lex.lex()
    return lexer