import os
import sys
import argparse
import atexit
import readline
from lexer import init_lexer
from parser import init_parser
from myast import *

DEBUG = False  # Debug mode is off by default

# === 4. File Analysis and Execution ===

# @{
# @brief Executes a source file written in the Draw++ language.
# @param file_path The path to the Draw++ source file.
def run_file(file_path):
    """Executes a Draw++ source file."""

    lexer = init_lexer()
    parser = init_parser()

    if not os.path.exists(file_path):
        print_error(f"The file '{file_path}' does not exist.")
        return

    with open(file_path, 'r') as file:
        filetxt = file.read()

    if DEBUG:
        print(f"[DEBUG] File : {file_path}")
        print(f"[DEBUG] Content :\n")
        lines = filetxt.splitlines()
        for i, line in enumerate(lines, start=1):
            print(f"[DEBUG] {i}\t{line}")

    # Lexical Analysis
    lexer.input(filetxt)
    tokens = list(lexer)

    if DEBUG:
        print(f"\n[DEBUG] TOKENS :")
        for token in tokens:
            print(f"[DEBUG] {str(token).replace('LexToken', '')}")  # Print tokens

    # Syntax Analysis
    lexer.lineno = 1  # Reset the line number
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
        print_error(f"Error during parsing: {e}")
        return

    if ast:
        # Execute the AST
        execute_ast(ast, DEBUG)
# @}

# === 5. Interactive Mode ===

# @{
# @brief Launches an interactive mode for on-the-fly drawing.
def run_interactive():
    """Interactive mode for on-the-fly drawing."""
    print("Interactive mode: type 'e' or 'q' to quit.")
    print("Special commands:")
    print("  - 'c' : Clear the screen")
    print("  - 'r' : Restart the interpreter")

    # History configuration
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

        # Lexical Analysis
        lexer.input(text)
        tokens = list(lexer)

        if DEBUG:
            print(f"[DEBUG] Tokens : {tokens}")

        # Syntax Analysis
        try:
            ast = parser.parse(text, lexer=lexer)
            if DEBUG:
                print(f"[DEBUG] AST : {ast}")
        except Exception as e:
            print_error(f"Error during parsing: {e}")
            continue
# @}

# === 6. Main Entry Point ===

# @{
# @brief Main entry point of the Draw++ interpreter.
# @details Handles command-line arguments for file execution or interactive mode.
def main():
    # DEBUG
    sys.argv.append("-d")
    sys.argv.append(".to_run.dpp")

    """Main entry point for the Draw++ interpreter."""
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
# @}

if __name__ == "__main__":
    main()