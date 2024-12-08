import ply.lex as lex
import ply.yacc as yacc

# ---- Analyse lexicale (Lexer) ----

# Liste des tokens
tokens = (
    'LINE',
    'CIRCLE',
    'NUMBER',
    'LOUAYE',
    'LPAREN',
    'RPAREN',
)

# Définition des règles pour chaque token
t_LINE = r'line'
t_CIRCLE = r'circle'
t_LOUAYE = r'louaye|LOUAYE'
t_LPAREN = r'\('
t_RPAREN = r'\)'

# Les nombres (ex : coordonnées ou rayon)
def t_NUMBER(t):
    r'\d+'
    t.value = int(t.value)
    return t

# Ignorer les espaces
t_ignore = ' \t'

# Gestion des erreurs lexicographiques
def t_error(t):
    print(f"Caractère invalide : {t.value[0]}")
    t.lexer.skip(1)

# Initialisation du lexer
lexer = lex.lex()

# ---- Analyse syntaxique (Parser) ----

# Grammaire
def p_command_line(p):
    'command : LINE LPAREN NUMBER NUMBER RPAREN'
    p[0] = f"SDL_RenderDrawLine(renderer, 0, 0, {p[3]}, {p[4]});"

def p_command_circle(p):
    'command : CIRCLE NUMBER'
    p[0] = f"draw_circle(renderer, {p[2]});"

def p_command_louaye(p):
    'command : LOUAYE NUMBER LINE'
    p[0] = f"Louaye a une line de {p[2]} cm"

# Gestion des erreurs syntaxiques
def p_error(p):
    print("Erreur de syntaxe !")

# Initialisation du parser
parser = yacc.yacc()

# ---- Point d'entrée ----

def main():
    print("Langage Draw++ Compiler")
    while True:
        try:
            s = input("draw++ > ")  # Lire une commande utilisateur
        except EOFError:
            break
        if not s:
            continue
        result = parser.parse(s, lexer=lexer)  # Parser l'entrée utilisateur
        if result:
            print("Code généré :")
            print(result)

if __name__ == "__main__":
    main()