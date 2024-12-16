# ,——————————————————————————————————————————————————————,
# |                                                      |
# |                    DRAW++ COMPILER                   |
# |                                                      |
# '——————————————————————————————————————————————————————'

############### PLAN ###############
 
# 1 - ERREURS : gestion des erreurs
# 2 - LEXER : Transforme le texte en une liste de tokens reconnaissables par le langage draw++ (analyse lexicale)
# 3 - PARSER : Analyse les tokens pour créer l'AST (analyse syntaxique)
# 4 - AST : Représentation structurée des instructions dpp (arbre syntaxique abstrait)
# 5 - génération de code C :  Traduit l'AST en code C intermédiaire
# 6 - exécution principale

###############################################################
#   Interpreter to run draw++ in terminal                     #
###############################################################

import os
import sys
from lexer import Lexer
from parser import Parser 
from errors_codegen import *  

# Fonction d'affichage des erreurs
def print_error(error):
    print(f"Erreur : {error.as_string()}")

# Fonction pour exécuter l'AST
def execute_ast(ast):
    """Exécute l'AST généré."""    
    if not ast:
        print("Aucune instruction à exécuter.")
        return
    
    for node in ast:
        # Générer le code C intermédiaire pour chaque instruction de l'AST
        c_code = generate_c_code(node)
        print(c_code)  # Ou, mieux, exécuter ce code C si nécessaire (par exemple, dans une machine virtuelle)

# Fonction pour analyser et exécuter un fichier source
def run_file(file_path):
    """Exécute un fichier source draw++."""
    if not os.path.exists(file_path):
        print(f"Erreur : Le fichier '{file_path}' n'existe pas.")
        return
    
    with open(file_path, 'r') as file:
        filetxt = file.read()

    # Créez directement l'instance de Lexer et obtenez les tokens
    lexer = Lexer(file_path, filetxt)
    tokens, error = lexer.make_tokens()  # Effectuer la tokenisation ici
    if error:
        print_error(error)
        return

    # Parse les tokens pour obtenir un AST
    parser = Parser(tokens)
    ast, error = parser.parse()
    
    if error:
        print_error(error)
        return

    # Exécuter l'AST
    execute_ast(ast)

# Fonction pour exécuter un code en ligne de commande (mode interactif)
def run_interactive():
    """Mode interactif pour dessiner à la volée."""
    print("Mode interactif : tapez 'exit' pour quitter.")
    while True:
        text = input('draw++ > ')
        if text.lower() == "exit":
            break
        elif text.lower() == "clear":
            os.system('clear' if os.name == 'posix' else 'cls')
        elif text.lower() == "restart":
            print("Redémarrage de l'interpréteur...")
            os.system('clear' if os.name == 'posix' else 'cls')
            continue

        # Créez directement l'instance de Lexer et obtenez les tokens
        lexer = Lexer('<stdin>', text)
        tokens, error = lexer.make_tokens()  # Effectuer la tokenisation ici
        if error:
            print_error(error)
            continue

        # Parsing
        parser = Parser(tokens)
        ast, error = parser.parse()

        if error:
            print_error(error)
            continue

        # Exécution de l'AST
        execute_ast(ast)

# Fonction principale d'exécution
def main():
    # DEBUG
    file_path = 'debug.dpp'
    run_file(file_path)

    """Point d'entrée principal pour l'interpréteur draw++."""
    # if len(sys.argv) > 1:  # Si un fichier est spécifié
    #     file_path = sys.argv[1]
    #     run_file(file_path)
    # else:  # Mode interactif
    #     run_interactive()

# Point d'entrée du programme
if __name__ == "__main__":
    main()