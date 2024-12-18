import os
import sys
import argparse
import atexit
import readline
from lexer import init_lexer
from parser import init_parser
from myast import *

DEBUG = False  # Debug mode is off by default

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

    if DEBUG:
        print(f"[DEBUG] File reading... : {file_path}")
        print(f"[DEBUG] Content :\n")

        lines = filetxt.splitlines()
        for i,line in enumerate(lines, start=1):
            print(f"[DEBUG] {i}\t{line}")

    # Analyse lexicale
    lexer.input(filetxt)
    tokens = list(lexer)

    if DEBUG:
        print(f"\n[DEBUG] TOKENS :")
        for token in tokens:
            print(f"[DEBUG] {str(token).replace("LexToken","")}") # Simple way to print tokens
        
    # Analyse syntaxique
    try:
        ast = parser.parse(filetxt, lexer)
        if DEBUG:
            print(f"\n[DEBUG] AST :")
        if isinstance(ast, list):
            for node in ast:
                print(f"[DEBUG] {node}")
        elif ast is not None:
            print(f"[DEBUG] Single AST Node: {ast}")
    except Exception as e:
        print_error(f"Erreur pendant le parsing : {e}")
        return

    if ast:
        # Exécution de l'AST
        execute_ast(ast, DEBUG)

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

        if DEBUG:
            print(f"[DEBUG] Tokens : {tokens}")

        # Analyse syntaxique
        try:
            ast = parser.parse(text, lexer=lexer)
            if DEBUG:
                print(f"[DEBUG] AST : {ast}")
        except Exception as e:
            print_error(f"Erreur pendant le parsing : {e}")
            continue

# === 6. Point d'entrée principal ===
def main():
    sys.argv.append("-d")
    sys.argv.append("test.dpp")

    argparser = argparse.ArgumentParser(description="Draw++ Language Interpreter")
    argparser.add_argument("file", nargs="?", help="Source file to execute (optional)")
    argparser.add_argument("-d", "--debug", action="store_true", help="Enable debug mode")
    args = argparser.parse_args()

    # Set the debug mode flag globally
    global DEBUG
    DEBUG = args.debug

    if DEBUG:
        print("[DEBUG] Debug mode activated.\n")

    if args.file:  # If a file is specified
        file_path = args.file
        run_file(file_path)
    else:  # Interactive mode
        run_interactive()

if __name__ == "__main__":
    main()