import ply.lex as lex

# === 1. Définition des tokens ===

keywords = {
    'if': 'IF',
    'else': 'ELSE',
    'for': 'FOR',
    'while': 'WHILE',
    'do': 'DO',
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

tokens = [
    'IDENTIFIER', 'NUMBER', 'STRING', 'BOOLEAN',
    'PLUS', 'MINUS', 'TIMES', 'DIVIDE', 'EQUALS',
    'LT', 'GT', 'LE', 'GE', 'EQ', 'NEQ', 'AND', 'OR', 'NOT',
    'LPAREN', 'RPAREN', 'LBRACE', 'RBRACE', 'LBRACKET', 'RBRACKET',
    'COMMA', 'SEMICOLON', 'COLON',
] + list(keywords.values())

# === 2. Règles des tokens ===

# Opérateurs
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

# Séparateurs
t_LPAREN = r'\('
t_RPAREN = r'\)'
t_LBRACE = r'\{'
t_RBRACE = r'\}'
t_LBRACKET = r'\['
t_RBRACKET = r'\]'
t_COMMA = r','
t_SEMICOLON = r';'
t_COLON = r':'

# Identifiants
def t_IDENTIFIER(t):
    r'[a-zA-Z_][a-zA-Z0-9_]*'
    t.type = keywords.get(t.value, 'IDENTIFIER')
    return t

# Nombres
def t_NUMBER(t):
    r'\d+(\.\d+)?'
    t.value = float(t.value) if '.' in t.value else int(t.value)
    return t

# Chaînes
def t_STRING(t):
    r'\"([^\\\"]|\\.)*\"'
    t.value = t.value[1:-1]
    return t

# Booléens
def t_BOOLEAN(t):
    r'True|False'
    t.value = (t.value == 'True')
    return t

# Ignorer espaces et tabulations
t_ignore = ' \t'

# Gestion des lignes
def t_newline(t):
    r'\n+'
    t.lexer.lineno += len(t.value)

# Gestion des erreurs lexicales
def t_error(t):
    print(f"\033[31mErreur lexicale : caractère illégal '{t.value[0]}' à la ligne {t.lineno}, colonne {find_column(t.lexer.lexdata, t.lexpos)}\033[0m")
    t.lexer.skip(1)

# Trouver la colonne où se situe l'erreur
def find_column(input_text, lexpos):
    last_newline = input_text.rfind('\n', 0, lexpos)
    return (lexpos - last_newline) if last_newline >= 0 else lexpos + 1

# Construire le lexer
def init_lexer():
    lexer = lex.lex()
    return lexer