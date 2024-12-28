import re
import ply.lex as lex
from difflib import get_close_matches

def suggest_keyword(word):
    suggestions = get_close_matches(word, keywords.keys(), n=1, cutoff=0.6)
    return suggestions[0] if suggestions else None

def get_known_identifiers(t):
    known_identifiers = []

    text = t.lexer.lexdata
    lines = text.split('\n')
    lines = [x for x in lines if x != ""] # supprimes toutes les lignes vides
    for line in lines :
        line = line.strip('\n')
        line = line.strip('\t')

        resultat = re.search(r'var (\w+)\s*=', line)
        if resultat:
            known_identifiers.append(resultat.group(1))

        resultat = re.search(r'func\s+(\w+)\s*\(', line)
        if resultat:
            known_identifiers.append(resultat.group(1))

        resultat = re.search(r'int\s+(\w+)[\s,)]', line)
        if resultat:
            known_identifiers.append(resultat.group(1))

        resultat = re.search(r'float\s+(\w+)[\s,)]', line)
        if resultat:
            known_identifiers.append(resultat.group(1))

        resultat = re.search(r'char\s+(\w+)', line)
        if resultat:
            known_identifiers.append(resultat.group(1))

    return known_identifiers

# === 1. Definition of Tokens ===

# @{
# @brief Reserved keywords and their corresponding token names
keywords = {
    'if': 'IF',
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
    'move': 'MOVE',
    'color': 'COLOR',
    'animate': 'ANIMATE',
    'rotate': 'ROTATE',
    'var': 'VARIABLE',
    'line': 'LINE',
    'circle': 'CIRCLE',
    'square': 'SQUARE',
    'arc': 'ARC',
    'point': 'POINT',
}
# @}

# @{
# @brief List of all token names, including operators, separators, and keywords
tokens = [
    'IDENTIFIER', 'NUMBER', 'STRING', 'BOOLEAN',
    'PLUS', 'MINUS', 'TIMES', 'DIVIDE', 'EQUALS',
    'LT', 'GT', 'LE', 'GE', 'EQ', 'NEQ', 'AND', 'OR', 'NOT',
    'LPAREN', 'RPAREN', 'LBRACE', 'RBRACE', 'LBRACKET', 'RBRACKET',
    'COMMA', 'SEMICOLON', 'COLON',
] + list(keywords.values())
# @}

# === 2. Token Rules ===

# @{
# @brief Regular expressions for operators
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
t_AND = r'and'
t_OR = r'or'
t_NOT = r'not'
# @}

# @{
# @brief Regular expressions for separators
t_LPAREN = r'\('
t_RPAREN = r'\)'
t_LBRACE = r'\{'
t_LBRACKET = r'\['
t_RBRACKET = r'\]'
t_COMMA = r','
t_COLON = r':'
# @}

def t_RBRACE(t):
    r'\}'
    return t

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
    elif t.value in ['True', 'False']:  # Handle boolean literals
        t.type = 'BOOLEAN'
        t.value = (t.value == 'True')
    elif t.value in known_identifiers:
        t.type = 'IDENTIFIER'
    else:
        # Check for similarity with keywords
        suggestion = suggest_keyword(t.value)
        if suggestion:
            raise SyntaxError(f"LexicalError: Unknown identifier '{t.value}' at line {t.lexer.lineno}, column {find_column(t.lexer.lexdata, t.lexpos)}")
        raise SyntaxError(f"LexicalError: Unknown identifier '{t.value}' at line {t.lexer.lineno}, column {find_column(t.lexer.lexdata, t.lexpos)}")
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
    r'True|False'
    t.value = (t.value == 'True')
    return t

# @brief Ignores spaces and tabs
t_ignore = ' \t'

# @brief Tracks line numbers for newline characters
# @param t Token object containing the matched value
def t_newline(t):
    r'\n+'
    t.lexer.lineno += len(t.value)

# @brief Handles single-line comments starting with '#'
# @param t Token object containing the matched value
def t_COMMENT(t):
    r'\#.*'
    t.lexer.lineno += t.value.count('\n')
    pass

# @brief Handles multi-line comments enclosed in /* */
# @param t Token object containing the matched value
def t_COMMENT_MULTILINE(t):
    r'/\*([^*]|\*[^/])*\*/'
    t.lexer.lineno += t.value.count('\n')
    pass

# @brief Handles lexical errors by printing the offending character and skipping it
# @param t Token object containing the unmatched value
def t_error(t):
    raise Exception(f"LexicalError : illegal character '{t.value[0]}' at line {t.lineno}, column {find_column(t.lexer.lexdata, t.lexpos)}")

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