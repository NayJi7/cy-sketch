#######################################
# IMPORTS
#######################################

from tokens import *
import re

#######################################
# REGEX PATTERNS
#######################################

def algoArg(matchgrp, tokens):
    try:
        # Tentative de conversion en entier
        value = int(matchgrp)
        tokens.append(Token(USER_TOKENS['INT'], value))
    except ValueError:
        try:
            # Si l'entier échoue, tenter une conversion en float
            value = float(matchgrp)
            tokens.append(Token(USER_TOKENS['FLOAT'], value))
        except ValueError:
            # Si l'int et le float échouent, c'est une variable
            tokens.append(Token(USER_TOKENS['VAR'], matchgrp))     

def cursor(text,tokens):
    # CURSOR name(x,y)
    pattern = r"(CURSOR)\s+([A-Za-z][A-Za-z0-9]*)\((-?\d+\.\d+|-?\d+|[A-Za-z][A-Za-z0-9]*),\s*(-?\d+\.\d+|-?\d+|[A-Za-z][A-Za-z0-9]*)\)"
    match = re.search(pattern, text)

    if match:
        # Vérifie si l'identifiant est un mot-clé
        type_ = LANG_TOKENS.get(match.group(1))
        tokens.append(Token(type_, None))
        
        # Vérifie si l'identifiant est un mot-clé
        type_ = USER_TOKENS.get(match.group(2), USER_TOKENS['IDENTIFIER'])
        tokens.append(Token(type_, match.group(2)))

        tokens.append(Token(SYMBOL_TOKENS['LPAREN'], None))
        algoArg(match.group(3), tokens) # append un arg de type int, float ou var
        algoArg(match.group(4), tokens)
        tokens.append(Token(SYMBOL_TOKENS['RPAREN'], None))
        tokens.append(Token(SYMBOL_TOKENS['EOL'], None))

def drawcircle(text, tokens):
    # DRAWCIRCLE name(x, y, radius, COLOR, type)
    pattern = r"(DRAWCIRCLE)\s+([A-Za-z][A-Za-z0-9]*)\s*\(\s*(-?\d+\.\d+|-?\d+|[A-Za-z][A-Za-z0-9]*)\s*,\s*(-?\d+\.\d+|-?\d+|[A-Za-z][A-Za-z0-9]*)\s*,\s*(-?\d+\.\d+|-?\d+|[A-Za-z][A-Za-z0-9]*)\s*,\s*([A-Z]+)\s*,\s*([a-zA-Z]+)\s*\)"
    match = re.search(pattern, text)

    if match:
        # Vérifie si l'identifiant est un mot-clé
        tokens.append(Token(LANG_TOKENS.get(match.group(1)), None))

        # Vérifie si l'identifiant est un mot-clé
        tokens.append(Token(USER_TOKENS.get('IDENTIFIER'), match.group(2)))

        algoArg(match.group(3), tokens)
        algoArg(match.group(4), tokens)
        algoArg(match.group(5), tokens)

        # Vérifie si le group 6 est une color
        tokens.append(Token(USER_TOKENS.get('COLOR'), match.group(6)))

        tokens.append(Token(USER_TOKENS.get('STR'), match.group(7)))
        tokens.append(Token(SYMBOL_TOKENS['EOL'], None))