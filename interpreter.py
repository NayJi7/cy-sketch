import os
import sys
import atexit
import platform
import ply.lex as lex
import ply.yacc as yacc
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
        filelines = filetxt.split('\n')  # Diviser le contenu en lignes
        
        non_comment_lines = []  # Liste pour stocker les lignes valides
        in_multiline_comment = False  # Indicateur pour savoir si on est dans un commentaire multi-ligne
        
        for line in filelines:
            stripped_line = line.strip()
            
            if not stripped_line:  # Ignorer les lignes vides
                continue
            
            # Gestion des blocs de commentaires multi-lignes
            if in_multiline_comment:
                # Vérifier si le bloc de commentaire se termine sur cette ligne
                if '*/' in stripped_line:
                    in_multiline_comment = False
                    # Compter tout ce qui est après "*/" s'il y a du code
                    code_after_comment = stripped_line.split('*/', 1)[1].strip()
                    if code_after_comment:
                        non_comment_lines.append(code_after_comment)
                continue
            
            # Vérifier si la ligne commence un bloc de commentaire multi-ligne
            if '/*' in stripped_line:
                in_multiline_comment = True
                # Compter tout ce qui est avant "/*" s'il y a du code
                code_before_comment = stripped_line.split('/*', 1)[0].strip()
                if code_before_comment:
                    non_comment_lines.append(code_before_comment)
                continue
            
            # Gestion des commentaires simples (commençant par #)
            if stripped_line.startswith('#'):
                continue
            
            # Si la ligne contient un commentaire inline, compter uniquement le code avant le #
            if '#' in stripped_line:
                code_before_inline_comment = stripped_line.split('#', 1)[0].strip()
                if code_before_inline_comment:
                    non_comment_lines.append(code_before_inline_comment)
                continue
            
            # Si la ligne n'est pas un commentaire ou vide, elle est valide
            non_comment_lines.append(stripped_line)
        
        fileline_count = len(non_comment_lines)  # Compter uniquement les lignes valides

    # DEBUG print(f"Nombre de lignes non commentées : {fileline_count}")

    # Analyse lexicale
    lexer.input(filetxt)
    tokens = list(lexer)

    # Analyse syntaxique
    try:
        ast = parser.parse(filetxt, lexer=lexer)
    except Exception as e:
        print_error(f"Erreur pendant le parsing : {e}")
        return

    if(ast):
        # Exécution de l'AST
        execute_ast(ast, len(ast) == fileline_count)

# === 5. Mode interactif ===
def run_interactive():
    """Mode interactif pour dessiner à la volée."""
    print("Mode interactif : tapez 'e ou q' pour quitter.")
    print("Commandes spéciales :")
    print("  - 'c' : Effacer l'écran")
    print("  - 'r' : Relancer l'interpréteur")

    # Configuration de l'historique
    history_file = os.path.expanduser("~/.drawpp_history")
    
    ############################### A RAJOUTER POUR FAIRE MARCHER SUR WINDOWS
    if platform.system() == "Windows":
        try:
            import pyreadline3 as readline  # pyreadline3 pour Windows
        except ImportError:
            print("Veuillez installer pyreadline3 avec 'pip install pyreadline3'")
            readline = None
    else:
        import readline

    #si on est sur macos ou linux
    if readline:
        history_file = os.path.expanduser("~/.python_history")
        if os.path.exists(history_file):
            readline.read_history_file(history_file)
        atexit.register(readline.write_history_file, history_file)
        readline.set_history_length(1000)
    #############################################
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
        execute_ast(ast, ast != None)

# === 6. Point d'entrée principal ===
def main():

    # DEBUG
    # if platform.system() == "Windows": # Windows
    #     sys.argv.append('./to_run/to_execute.dpp')
    # elif platform.system() == "Linux": #Linux ou Mac
    #     sys.argv.append('to_run/to_execute.dpp')
    # elif platform.system() == "Darwin":
    #     sys.argv.append('to_run/to_execute.dpp')

    #pour windows
    sys.argv.append('./.to_run/to_execute.dpp')

    if len(sys.argv) > 1:  # Si un fichier est spécifié
        file_path = sys.argv[1]
        run_file(file_path)
    else:  # Mode interactif
        run_interactive()

if __name__ == "__main__":
    main()