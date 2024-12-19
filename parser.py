import os
import sys
import ply.yacc as yacc
from lexer import tokens, find_column

# === 1. Règles du parser ===

def p_programme(p):
    '''programme : instruction
                 | programme instruction'''
    p[0] = [p[1]] if len(p) == 2 else p[1] + [p[2]]

def p_instruction(p):
    '''instruction : dessin
                   | deplacement
                   | rotation
                   | couleur
                   | assignation
                   | modification
                   | conditionnelle
                   | boucle
                   | bloc'''
    p[0] = p[1]

def p_dessin(p):
    '''dessin : DRAW forme LPAREN parametres RPAREN'''
    p[0] = ('draw', p[2], p[4])

def p_forme(p):
    '''forme : LINE
             | CIRCLE
             | SQUARE
             | ARC
             | POINT'''
    p[0] = p[1]

def p_parametres(p):
    '''parametres : parametres COMMA arg
                  | arg'''
    p[0] = p[1] + [p[3]] if len(p) == 4 else [p[1]]

def p_arg(p):
    '''arg : NUMBER
           | IDENTIFIER
           | STRING
           | BOOLEAN
           | expression_arithmetic'''
    p[0] = p[1]

def p_expression_arithmetic(p):
    '''expression_arithmetic : expression_arithmetic PLUS expression_arithmetic
                             | expression_arithmetic MINUS expression_arithmetic
                             | expression_arithmetic TIMES expression_arithmetic
                             | expression_arithmetic DIVIDE expression_arithmetic'''
    # Handle binary operations
    p[0] = ('op', p[2], p[1], p[3])

def p_expression_arithmetic_atomic(p):
    '''expression_arithmetic : LPAREN expression_arithmetic RPAREN
                             | NUMBER
                             | IDENTIFIER'''
    # Handle atomic expressions (parentheses, numbers, identifiers)
    p[0] = p[1] if len(p) == 2 else p[2]

def p_deplacement(p):
    '''deplacement : MOVE LPAREN arg COMMA arg RPAREN'''
    p[0] = ('move', p[3], p[5])

def p_rotation(p):
    '''rotation : ROTATE LPAREN arg RPAREN'''
    p[0] = ('rotate', p[3])

def p_couleur(p):
    '''couleur : COLOR LPAREN arg RPAREN'''
    p[0] = ('color', p[3])

def p_assignation(p):
    '''assignation : VARIABLE IDENTIFIER EQUALS arg'''
    p[0] = ('assign', p[2], p[4])

def p_modification(p):
    '''modification : IDENTIFIER EQUALS arg'''
    p[0] = ('modify', p[1], p[3])

def p_valeur(p):
    '''valeur : NUMBER
              | IDENTIFIER
              | STRING
              | BOOLEAN'''
    p[0] = p[1]

def p_bloc(p):
    '''bloc : LBRACE programme RBRACE'''
    p[0] = ('block', p[2])

def p_conditionnelle(p):
    '''conditionnelle : IF LPAREN expression_logique RPAREN bloc ELSE bloc
                      | IF LPAREN expression_logique RPAREN bloc'''
    p[0] = ('if', p[3], p[5], p[7]) if len(p) == 8 else ('if', p[3], p[5])

def p_expression_logique(p):
    '''expression_logique : valeur operateur_comparaison valeur
                          | valeur operateur_comparaison BOOLEAN
                          | BOOLEAN operateur_comparaison valeur
                          | BOOLEAN'''
    p[0] = (p[2], p[1], p[3]) if len(p) == 4 else p[1]

def p_operateur_comparaison(p):
    '''operateur_comparaison : EQ
                             | NEQ
                             | LT
                             | GT
                             | LE
                             | GE'''
    p[0] = p[1]

def p_boucle(p):
    '''boucle : FOR LPAREN assignation SEMICOLON expression_logique SEMICOLON modification RPAREN bloc'''
    p[0] = ('for', p[3], p[5], p[7], p[9])

# === 2. Gestion des erreurs syntaxiques ===

def p_error(p):
    if p:
        print(f"\033[31mErreur syntaxique : '{p.value}' à la ligne {p.lineno}, colonne {find_column(p.lexer.lexdata, p.lexpos)}\033[0m")
        if p.type == "RPAREN":
            print("\033[34mSuggestion : Vérifiez que toutes les parenthèses ouvrantes '(' ont une parenthèse fermante correspondante ')'.\033[0m")
        elif p.type == "SEMICOLON":
            print("\033[34mSuggestion : Vérifiez les points-virgules ';'.\033[0m")
    else:
        print("\033[31mErreur syntaxique : fin de fichier inattendue.\033[0m")

# === 3. Construire le parser avec fichiers générés dans 'generated' ===
def init_parser():
    # Dossier des fichiers générés
    generated_folder = os.path.join(os.path.dirname(__file__), '.parser_generated')

    # Créer le dossier s'il n'existe pas
    if not os.path.exists(generated_folder):
        os.makedirs(generated_folder)

    # Ajouter le dossier généré dans le sys.path pour qu'il soit accessible comme module
    if generated_folder not in sys.path:
        sys.path.insert(0, generated_folder)

    # Spécifier un nom de module relatif pour parsetab
    parser = yacc.yacc(
        debug=True,
        outputdir=generated_folder,
        tabmodule="parsetab"
    )

    return parser