import os
import sys
import atexit
import readline
from lexer import init_lexer
from parser import init_parser
from myast import *

# === 4. Analyse et exécution d'un fichier source ===
def run_file(file_path):
    """Exécute un fichier source draw++."""

    lexer = init_lexer()
    parser = init_parser()

    if not os.path.exists(file_path):
        print_error(f"Le fichier '{file_path}' n'existe pas.")
        return

    with open(file_path, 'r') as file:
        filetxt = file.read()

    # Analyse lexicale
    lexer.input(filetxt)

    # Analyse syntaxique
    try:
        ast = parser.parse(filetxt, lexer)
    except Exception as e:
        print_error(f"Erreur pendant le parsing : {e}")
        return

    if(ast):
        # Exécution de l'AST
        execute_ast(ast)

# === 5. Mode interactif ===
def run_interactive():
    """Mode interactif pour dessiner à la volée."""
    print("Mode interactif : tapez 'e ou q' pour quitter.")
    print("Commandes spéciales :")
    print("  - 'c' : Effacer l'écran")
    print("  - 'r' : Relancer l'interpréteur")

    # Configuration de l'historique
    history_file = os.path.expanduser("~/.drawpp_history")
    if os.path.exists(history_file):
        readline.read_history_file(history_file)
    atexit.register(readline.write_history_file, history_file)
    readline.set_history_length(1000)

    lexer = init_lexer()
    parser = init_parser()

    while True:
        try:
            text = input('draw++ > ')
        except EOFError:
            break

        if text.lower() in ("e", "q"):
            break
        elif text.lower() == "c":
            os.system('clear' if os.name == 'posix' else 'cls')
            continue
        elif text.lower() == "r":
            os.system(f'python3 {__file__}')
            break
        elif text.strip() == "":
            continue

        # Analyse lexicale
        lexer.input(text)
        tokens = list(lexer)

        # Analyse syntaxique
        try:
            ast = parser.parse(text, lexer=lexer)
        except Exception as e:
            print_error(f"Erreur pendant le parsing : {e}")
            continue

        # Exécution de l'AST
        execute_ast(ast)

# === 6. Point d'entrée principal ===
def main():

    # DEBUG
    sys.argv.append('/home/cytech/gitstore/cy-sketch/Deulyne/test.dpp')

    if len(sys.argv) > 1:  # Si un fichier est spécifié
        file_path = sys.argv[1]
        run_file(file_path)
    else:  # Mode interactif
        run_interactive()

if __name__ == "__main__":
    main()