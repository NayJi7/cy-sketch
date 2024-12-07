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

        for line in self.lines :
            result = cursor(line, tokens)
            result = drawcircle(line, tokens)

            self.ln += 1

        tokens.append(Token(SYMBOL_TOKENS['EOF'], None))
        return tokens, None