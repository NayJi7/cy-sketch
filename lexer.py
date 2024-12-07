#######################################
# IMPORTS
#######################################

from tokens import *
from regex_process import *
from errors_codegen import *  # Contient les classes d'erreurs, comme InvalidSyntaxError, IllegalCharError, etc.

#######################################
# LEXER
#######################################

class Lexer:
    """Analyseur lexical qui convertit le texte source en une liste de tokens."""
    
    def __init__(self, fn, text):
        self.fn = fn
        self.ln = 0
        self.lines = text.split("\n")
    
    def make_tokens(self):
        """Convertit tout le texte source en une liste de tokens utilisables par le parser."""
        tokens = []
        error = None

        for ln, line in enumerate(self.lines) :
            self.ln = ln+1
            found = False

            error = cursor(ln+1, line, tokens)
            if error and not found:
                return None, error
            elif not error :
                found = True
            
            error = drawcircle(ln+1, line, tokens)
            if error and not found:
                return None, error
            elif not error:
                found = True
            
        tokens.append(Token(SYMBOL_TOKENS['EOF'], None))
        return tokens, error