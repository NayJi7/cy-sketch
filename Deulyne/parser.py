import ply.yacc as yacc
from lexer import tokens, find_column

# === 1. Règles du parser ===

def p_programme(p):
    '''programme : instruction
                 | programme instruction'''
    p[0] = [p[1]] if len(p) == 2 else p[1] + [p[2]]

def p_instruction(p):
    '''instruction : dessin
                   | déplacement
                   | rotation
                   | couleur
                   | assignation
                   | conditionnelle
                   | boucle
                   | bloc'''
    p[0] = p[1]

def p_dessin(p):
    '''dessin : DRAW forme LPAREN paramètres RPAREN'''
    p[0] = ('draw', p[2], p[4])

def p_forme(p):
    '''forme : "line"
             | "circle"
             | "square"
             | "arc"
             | "point"'''
    p[0] = p[1]

def p_paramètres(p):
    '''paramètres : paramètres COMMA NUMBER
                  | NUMBER'''
    p[0] = [p[1]] if len(p) == 2 else p[1] + [p[3]]

def p_déplacement(p):
    '''déplacement : MOVE LPAREN NUMBER COMMA NUMBER RPAREN'''
    p[0] = ('move', p[3], p[5])

def p_rotation(p):
    '''rotation : ROTATE LPAREN NUMBER RPAREN'''
    p[0] = ('rotate', p[3])

def p_couleur(p):
    '''couleur : COLOR LPAREN STRING RPAREN'''
    p[0] = ('color', p[3])

def p_assignation(p):
    '''assignation : VARIABLE IDENTIFIER EQUALS valeur'''
    p[0] = ('assign', p[2], p[4])

def p_valeur(p):
    '''valeur : NUMBER
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
    '''expression_logique : valeur opérateur_comparaison valeur'''
    p[0] = (p[2], p[1], p[3])

def p_opérateur_comparaison(p):
    '''opérateur_comparaison : EQ
                             | NEQ
                             | LT
                             | GT
                             | LE
                             | GE'''
    p[0] = p[1]

def p_boucle(p):
    '''boucle : FOR LPAREN assignation SEMICOLON expression_logique SEMICOLON assignation RPAREN bloc'''
    p[0] = ('for', p[3], p[5], p[7], p[9])

# === 2. Gestion des erreurs syntaxiques ===

def p_error(p):
    if p:
        print(f"\033[31mErreur syntaxique : '{p.value}' à la ligne {p.lineno}, colonne {find_column(p.lexer.lexdata, p.lexpos)}\033[0m")
        if p.type == "RPAREN":
            print("\033[32mSuggestion : Vérifiez que toutes les parenthèses ouvrantes '(' ont une parenthèse fermante correspondante ')'.\033[0m")
        elif p.type == "SEMICOLON":
            print("\033[32mSuggestion : Assurez-vous que chaque instruction se termine par un point-virgule ';'.\033[0m")
    else:
        print("\033[31mErreur syntaxique : fin de fichier inattendue.\033[0m")

# === 3. Construire le parser ===

parser = yacc.yacc()

if __name__ == "__main__":
    data = """
    variable x = 10;
    draw circle(50, 50, 10;
    move(10, 15)
    """
    result = parser.parse(data)
    print(result)
