import re

#######################################
# TOKENS ET TYPES
#######################################

LANG_TOKENS = {
    'DRAWCIRCLE': 'TT_DRAWCIRCLE',  # cercleColor()
}

SYMBOL_TOKENS = {
    'LPAREN': 'TT_LPAREN',   # (
    'RPAREN': 'TT_RPAREN',   # )
    'COMMA': 'TT_COMMA',     # ,
    'EOF': 'TT_EOF',         # Fin de fichier
}

#######################################
# TOKEN
#######################################

class Token:
    def __init__(self, type_, value):
        self.type = type_
        self.value = value
    
    def __repr__(self):
        if self.value:
            return f'{self.type}:{self.value}'
        return f'{self.type}'

#######################################
# LEXER
#######################################

class Lexer:
    def __init__(self, text):
        self.text = text

        self.token_specification = [
            ('TT_DRAWCIRCLE', r'\bcercleColor\b'),  # Mot-clé cercleColor
            ('TT_LPAREN', r'\('),                  # Parenthèse gauche
            ('TT_RPAREN', r'\)'),                  # Parenthèse droite
            ('TT_COMMA', r','),                    # Virgule
            ('TT_INT', r'\b[0-9]+\b'),                # Nombres entiers
            ('TT_FLOAT', r'\b[0-9]+\.[0-9]+\b'),         # Nombres flottants
            ('TT_STRING', r'\b(filled|empty)\b'),  # Type rempli ou vide
            ('TT_COLOR', r'0x[0-9A-Fa-f]{8}'),     # Couleurs en hexadécimal
            ('TT_WHITESPACE', r'[ \t]+'),          # Espaces blancs (ignorés)
        ]
        self.regex = re.compile('|'.join(f'(?P<{name}>{pattern})' 
                                         for name, pattern in self.token_specification))
    def make_tokens(self):
        tokens = []

        for match in self.regex.finditer(self.text):
            kind = match.lastgroup
            value = match.group(kind)

            if kind == 'TT_WHITESPACE':  # Ignorer les espaces
                continue

            tokens.append(Token(kind, value))
        
        tokens.append(Token(SYMBOL_TOKENS['EOF'], None))  # Ajouter un token EOF
        return tokens

#######################################
# PARSER
#######################################

class Parser:
    def __init__(self, tokens):
        self.tokens = tokens
        self.pos = 0
        self.current_token = self.tokens[self.pos]

    def advance(self):
        self.pos += 1
        self.current_token = self.tokens[self.pos] if self.pos < len(self.tokens) else None

    def parse(self):
        while self.current_token.type != SYMBOL_TOKENS['EOF']:
            if self.current_token.type == LANG_TOKENS['DRAWCIRCLE']:
                self.parse_draw_circle()
            else:
                print(f"Unknown token: {self.current_token}")
                self.advance()

    def parse_draw_circle(self):
        """
        Parse une commande cercleColor(x, y, radius, color, type)
        """
        self.advance()  # Passer le token TT_DRAWCIRCLE
        if self.current_token.type == SYMBOL_TOKENS['LPAREN']:
            self.advance()  # Passer '('

            # Lire x, y, radius, color, type
            x = self.expect_number()
            self.expect(SYMBOL_TOKENS['COMMA'])
            y = self.expect_number()
            self.expect(SYMBOL_TOKENS['COMMA'])
            radius = self.expect_number()
            self.expect(SYMBOL_TOKENS['COMMA'])
            color = self.expect_color()
            self.expect(SYMBOL_TOKENS['COMMA'])
            circle_type = self.expect_string()  # Type rempli ou vide

            self.expect(SYMBOL_TOKENS['RPAREN'])  # Passer ')'

            # Appeler la fonction associée
            self.execute_draw_circle(int(x), int(y), int(radius), color, circle_type)

    def expect(self, token_type):
        """
        Vérifie que le token courant est du type attendu, sinon lève une erreur.
        """
        if self.current_token.type == token_type:
            self.advance()
        else:
            raise SyntaxError(f"Expected {token_type}, got {self.current_token.type}")

    def expect_number(self):
        """
        Vérifie que le token courant est un nombre et retourne sa valeur.
        """
        if self.current_token.type in ['TT_INT', 'TT_FLOAT']:
            value = self.current_token.value
            self.advance()
            return value
        else:
            raise SyntaxError(f"Expected a number, got {self.current_token.type}")

    def expect_color(self):
        """
        Vérifie que le token courant est une couleur en hexadécimal et retourne sa valeur.
        """
        if self.current_token.type == 'TT_COLOR':
            value = self.current_token.value
            self.advance()
            return int(value, 16)  # Convertir la couleur en entier
        else:
            raise SyntaxError(f"Expected a color (0x...), got {self.current_token.type}")

    def expect_string(self):
        """
        Vérifie que le token courant est une chaîne et retourne sa valeur.
        """
        if self.current_token.type == 'TT_STRING':
            value = self.current_token.value
            self.advance()
            return value
        else:
            raise SyntaxError(f"Expected a string (filled/empty), got {self.current_token.type}")

    def execute_draw_circle(self, x, y, radius, color, circle_type):
        """
        Exécute la fonction associée à cercleColor.
        """
        print(f"Drawing circle at ({x}, {y}) with radius {radius}, color {hex(color)}, type {circle_type}")
        # Appelle ici une fonction SDL ou autre pour dessiner le cercle
        # Exemple d'utilisation : drawCircle(renderer, x, y, radius, color, circle_type)

#######################################
# EXEMPLE
#######################################

if __name__ == "__main__":
    code = "cercleColor(100, 200, 50, 0xFF00FF00, filled)"
    lexer = Lexer(code)
    tokens = lexer.make_tokens()

    print("Tokens:", tokens)

    parser = Parser(tokens)
    parser.parse()
