import re


""" ctypes est une bibliothèque Python qui permet d'appeler des fonctions C 
depuis Python en interagissant directement avec des bibliothèques partagées comme form.so. """
import ctypes


"""Cette ligne charge la bibliothèque partagée appelée libform.so. Cela signifie que 
toutes les fonctions définies dans ce fichier C sont maintenant accessibles depuis Python.
"""
form_lib = ctypes.CDLL('./files.lib/libform.so')


"""Cette ligne Spécifie les types d'arguments attendus par la fonction drawCircle. 
Cela garantit que Python sait quels types de données passer à la fonction C lors de son appel."""
form_lib.drawCircle.argtypes = [
    ctypes.c_void_p,  # SDL_Renderer* (pointeur vers le renderer)
    ctypes.c_int,     # x
    ctypes.c_int,     # y
    ctypes.c_int,     # radius
    ctypes.c_uint32,  # color (Uint32 pour la couleur)
    ctypes.c_char_p   # type ("filled"|"empty")
]
form_lib.drawCircle.restype = None  #  Indique que la fonction drawCircle ne retourne rien .

"""RAISON : Si tu ne définis pas argtypes, Python ne saura pas vérifier ou 
convertir automatiquement les arguments avant de les passer à la fonction C. 
Cela pourrait provoquer des erreurs de segmentation (segfault) ou des comportements imprévisibles."""

#######################################
# CONSTANTES
#######################################

# Caractères numériques pour les nombres
DIGITS = '0123456789'

# Tokens spécifiques au langage dpp
LANG_TOKENS = {
    'CURSOR': 'TT_CURSOR',  # Créer un curseur
    'MOVE': 'TT_MOVE',      # Déplacer un curseur
    'ROTATE': 'TT_ROTATE',  # Faire pivoter un curseur
    'COLOR': 'TT_COLOR',    # Changer la couleur
    'DRAWCIRCLE': 'TT_DRAWCIRCLE',
    'DRAWRECTANGLE': 'TT_DRAWRECTANGLE',
    'DRAWELLIPSE': 'TT_DRAWELLIPSE',
    'DRAWROUNDEDRECTANGLE': 'TT_DRAWROUNDEDRECTANGLE',
    'DRAWPOLYGON': 'TT_DRAWPOLYGON',
    'DRAWLINE': 'TT_DRAWLINE',
    'STRING': 'TT_STRING',
    'COLOR': 'TT_COLOR',
    'IF': 'TT_IF',          # Instruction conditionnelle
    'ELSE': 'TT_ELSE'       # Instruction alternative dans les conditions
}

# Types de tokens pour les identifiants et valeurs numériques
USER_TOKENS = {
    'IDENTIFIER': 'TT_IDENTIFIER',  # Noms de curseurs ou variables
    'INT': 'TT_INT',                # Nombre entier
    'FLOAT': 'TT_FLOAT',            # Nombre à virgule flottante
    'VAR': 'TT_VAR'                 # Noms de variables
}

# Tokens pour les symboles de syntaxe
SYMBOL_TOKENS = {
    'LPAREN': 'TT_LPAREN',   # (
    'RPAREN': 'TT_RPAREN',   # )
    'LBRACE': 'TT_LBRACE',   # {
    'RBRACE': 'TT_RBRACE',   # }
    'COMMA': 'TT_COMMA',   # ,
    'WHITESPACE': 'TT_WHITESPACE',
    'EOF': 'TT_EOF'          # Fin de fichier
}

# Tokens pour les operateurs
OPERATORS_TOKENS = {
    '+': 'TT_PLUS',
    '-': 'TT_MINUS',
    '*': 'TT_MULTIPLY', 
    '/': 'TT_DIVIDE', 
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
    "La classe Lexer initialise une série de motifs d'expressions régulières pour identifier différentes parties du texte, comme des commandes de dessin, des symboles, des nombres, des chaînes, etc. Elle compile ces motifs en une expression régulière unique pour améliorer les performances et éviter de réinitialiser les motifs à chaque analyse. Ensuite, la méthode make_tokens parcourt le texte à la recherche de toutes les correspondances, crée des objets Token pour chaque correspondance et les ajoute à une liste. Elle ignore les espaces blancs et termine la liste de tokens avec un token spécial EOF pour signaler la fin du texte."

    def __init__(self, fn, text):
        self.fn = fn
        self.text = text

    # Définition des commandes de dessin -----------------------------------------
        # Structure : (type de token, r'motif')

        self.draw_commands = [
            (LANG_TOKENS['DRAWCIRCLE'], r'\bdrawCircle\b'),  
            (LANG_TOKENS['DRAWRECTANGLE'], r'\bdrawRectangle\b'),
            (LANG_TOKENS['DRAWELLIPSE'], r'\bdrawEllipse\b'),
            (LANG_TOKENS['DRAWROUNDEDRECTANGLE'], r'\bdrawRoundedRectangle\b'),
            (LANG_TOKENS['DRAWPOLYGON'], r'\bdrawCustomPolygon\b'),
            (LANG_TOKENS['DRAWLINE'], r'\bdrawLine\b'),
            (LANG_TOKENS['STRING'], r'\b(filled|empty)\b'),  # Type rempli ou vide
            (LANG_TOKENS['COLOR'], r'0x[0-9A-Fa-f]{8}'),  
        ]

        self.symbol_tokens = [
            (SYMBOL_TOKENS['LPAREN'], r'\('),
            (SYMBOL_TOKENS['RPAREN'], r'\)'),                  
            (SYMBOL_TOKENS['COMMA'], r','),                          
            (SYMBOL_TOKENS['WHITESPACE'], r'[ \t]+'),          
        ]

        self.user_tokens = [
            (USER_TOKENS['INT'], r'\b[0-9]+\b'),                
            (USER_TOKENS['FLOAT'], r'\b[0-9]+\.[0-9]+\b'),    
        ]

        self.tokens = self.draw_commands + self.symbol_tokens + self.user_tokens
    #-----------------------------------------------------------------------------
    # Construction d'une regex unique pour reconnaître tous les tokens

        #re.compile : 
        "Définit un groupe nommé (l'association de par exemple cercleColor qui est ici TT_DRAWCIRCLE) pour chaque token, permettant de retrouver facilement le type du token (name) lors de l'analyse" 

        #.join : 
        "Combine les motifs regex de chaque token en les séparant par le caractère |"
            #?P<...> : Permet de nommer les groupes capturés dans une expression régulière. (par exemple, TT_DRAWCIRCLE, TT_INT, etc.). 
                # => Rend les résultats de la recherche plus lisibles et facilite l'accès aux groupes capturés"

        "Resume : optimiser les performances en précompilant les expressions régulières une seule fois, ce qui permet de réutiliser efficacement les mêmes regex pour identifier tous les tokens sans avoir besoin de les réinitialiser à chaque recherche"
        self.regex = re.compile('|'.join(f'(?P<{name}>{pattern})' 
                                         for name, pattern in self.tokens))
        
        # CELA NOUS DONNE : (?P<TT_DRAWCIRCLE>\bdrawCircle\b)|(?P<TT_LPAREN>\()|(?P<TT_INT>\b[0-9]+\b)

        
    def make_tokens(self):
        "Parcourt le texte et, pour chaque partie du texte qui correspond à un motif défini dans les expressions régulières, elle crée un token avec un type (comme TT_DRAWCIRCLE, TT_INT, etc.) et une valeur correspondante. Elle ignore les espaces blancs et ajoute un token spécial EOF à la fin pour signaler la fin de l'entrée. Ces tokens sont ensuite retournés sous forme de liste, prêts à être traités par le Parser"

        #tokens : Contient tous les tokens trouvés dans le texte.
        tokens = []

        #finditer : parcourt le texte à la recherche de toutes les correspondances avec les motifs définis dans regex (voir regex dans init)
        for match in self.regex.finditer(self.text):

        #kind : Renvoie le nom du lastgroup (le type de token) capturé dans l'expression régulière qui a trouvé une correspondance
            kind = match.lastgroup
        #value : Extrait la valeur capturée par le groupe nommé "kind" grace à la fonction group()
            value = match.group(kind)

            # =>  Si kind est "TT_DRAWCIRCLE", value sera la chaîne "drawCircle"

        # Ignorer les espaces
            if kind == 'TT_WHITESPACE':  
                continue
        
        # CE QUE CELA NOUS DONNE : [TT_DRAWCIRCLE:drawCircle, TT_LPAREN:(, ....]
            tokens.append(Token(kind, value))

        # Ajouter un token EOF à la fin
        tokens.append(Token(SYMBOL_TOKENS['EOF'], None))  
        return tokens


#######################################
# PARSER
#######################################

class Parser:
    def __init__(self, tokens):
        "Initialise le parser avec une liste de tokens."

        self.tokens = tokens
        self.pos = 0  # Position actuelle dans la liste de tokens
        self.current_token = self.tokens[self.pos]

    def advance(self):
        "Passe au token suivant dans la liste."

        self.pos += 1
        self.current_token = self.tokens[self.pos] if self.pos < len(self.tokens) else None

    def expect(self, expected_type):
        """ Vérifie que le token courant correspond au type attendu et effectue des conversions si nécessaire.

        - Si `expected_type` est 'TT_INT' ou 'TT_FLOAT', retourne un nombre.
        - Si `expected_type` est 'TT_COLOR', retourne une couleur convertie en entier.
        - Si `expected_type` est 'TT_STRING', retourne une chaîne.
        - Sinon, vérifie simplement le type sans conversion.
        """

        if self.current_token.type == expected_type:
            value = self.current_token.value

            # Conversion selon le type attendu
            if expected_type == 'TT_INT':
                value = int(value)
            elif expected_type == 'TT_FLOAT':
                value = float(value)
            elif expected_type == 'TT_COLOR':
                value = int(value, 16)  # Convertir couleur hexadécimale
            elif expected_type == 'TT_STRING':
                pass  # Aucun traitement supplémentaire requis pour les chaînes

            self.advance()  # Passe au token suivant
            return value
        else:
            raise SyntaxError(f"Expected {expected_type}, got {self.current_token.type}")

    def parse_draw_circle(self):
        """Cette méthode lit les arguments de drawCircle(x, y, radius, color, type), 
        vérifie leur validité grâce à des méthodes expect, et appelle une fonction 
        associée (execute_draw_circle) avec les valeurs extraites."""

        self.advance()  # Passe le token TT_DRAWCIRCLE
        #-----------------------------------------------------------------------------
        self.expect('TT_LPAREN')  # Vérifie et passe '('
        #-----------------------------------------------------------------------------
        x = self.expect('TT_INT')  # Lire x
        #-----------------------------------------------------------------------------
        self.expect('TT_COMMA')   # Vérifie et passe ','
        #-----------------------------------------------------------------------------
        y = self.expect('TT_INT')  # Lire y
        #-----------------------------------------------------------------------------
        self.expect('TT_COMMA')   # Vérifie et passe ','
        #-----------------------------------------------------------------------------
        radius = self.expect('TT_INT')  # Lire le rayon
        #-----------------------------------------------------------------------------
        self.expect('TT_COMMA')   # Vérifie et passe ','
        #-----------------------------------------------------------------------------
        color = self.expect('TT_COLOR')  # Lire la couleur
        #-----------------------------------------------------------------------------
        self.expect('TT_COMMA')   # Vérifie et passe ','
        #-----------------------------------------------------------------------------
        circle_type = self.expect('TT_STRING')  # Lire le type (filled ou empty)
        #-----------------------------------------------------------------------------
        self.expect('TT_RPAREN')  # Vérifie et passe ')'

        # Appeler la fonction associée
        self.execute_draw_circle(x, y, radius, color, circle_type)


    def parse(self):
        "Parse l'ensemble des tokens jusqu'à rencontrer EOF."

        while self.current_token.type != SYMBOL_TOKENS['EOF']:
            if self.current_token.type == LANG_TOKENS['DRAWCIRCLE']:
                # Si le token courant correspond à "drawCircle", appelle une méthode pour le traiter.
                self.parse_draw_circle()
            else:
                # Gère les cas où le token est inconnu ou non pris en charge.
                print(f"Unknown token: {self.current_token}")
                self.advance()  # Avance pour éviter une boucle infinie
                
        #---        
            #else if self.current_token.type == LANG_TOKENS['DRAWRECTANGLE']:
                #self.parse_draw_rectangle()
            #else if self.current_token.type == LANG_TOKENS['DRAWROUNDEDRECTANGLE']:
                #self.parse_draw_rounded_rectangle()
            #else if self.current_token.type == LANG_TOKENS['DRAWELLIPSE']:
                #self.parse_draw_ellipse()
            #else if self.current_token.type == LANG_TOKENS['DRAWPOLYGON']:
                #self.parse_draw_polygon()
            #else if self.current_token.type == LANG_TOKENS['DRAWLINE']:
                #self.parse_draw_line()
        #---        
            


    def execute_draw_circle(self, x, y, radius, color, circle_type):
        "Exécute l'action associée à la commande drawCircle avec les paramètres fournis"

        #print(f"Drawing circle at ({x}, {y}) with radius {radius}, color {hex(color)}, type {circle_type}")
        # Appelle ici une fonction SDL ou autre pour dessiner le cercle
        form_lib.drawCircle(None, x, y, radius, color, circle_type.encode('utf-8'))


#######################################
# FONCTION PRINCIPALE D'EXÉCUTION
#######################################

if __name__ == "__main__":
    code = "drawCircle(100, 200, 50, 0xFF00FF00, filled)"
    lexer = Lexer('<stdin>', code)
    tokens = lexer.make_tokens()

    print("\nTokens:", tokens)

    parser = Parser(tokens)
    parser.parse()
