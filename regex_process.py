from tokens import *
import re

def algoArg(matchgrp, tokens):
    count = 0

    try:
        # Tentative de conversion en entier
        int(matchgrp)
    except ValueError:
        count += 1
        pass

    if count == 0:
        tokens.append(Token(USER_TOKENS['INT'], int(matchgrp)))

    try:
        # Si la conversion en int échoue, tenter en float
        if count != 0:
            float(matchgrp)
        else :
            count=3 # is an int and has been append
    except ValueError:
        count += 1
        pass

    if count == 1:
        tokens.append(Token(USER_TOKENS['FLOAT'], float(matchgrp)))
        
    if count == 2:
        # Si les deux conversions échouent, alors c'est une chaîne
        tokens.append(Token(USER_TOKENS['VAR'], matchgrp))

def operations(text, tokens):
    pattern_operation = r"([A-Za-z0-9]+(?:\.[A-Za-z0-9]+)?)|([\+\-\*\/])"
    # Appliquer re.finditer pour obtenir des objets Match
    matches = re.finditer(pattern_operation, text)

    # Parcourir les correspondances
    for match in matches:
        if match.group(1):  # C'est un opérande
            try:
                value = int(match.group(1))  # Vérifier si c'est un entier
                tokens.append(Token(USER_TOKENS['INT'], value))
            except ValueError:
                try:
                    value = float(match.group(1))  # Vérifier si c'est un flottant
                    tokens.append(Token(USER_TOKENS['FLOAT'], value))
                except ValueError:
                    tokens.append(Token(USER_TOKENS['VAR'], match.group(1)))
        elif match.group(2):  # C'est un opérateur
            tokens.append(Token(OPERATORS_TOKENS[match.group(2)], match.group(2)))




def cursor(text,tokens,result):
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

        algoArg(match.group(3), tokens) # append un arg de type int, float ou var
        algoArg(match.group(4), tokens)
        return result+1
    else:
        return result

def drawcircle(text, tokens, result):
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
        return result+1
    else:
        return result
