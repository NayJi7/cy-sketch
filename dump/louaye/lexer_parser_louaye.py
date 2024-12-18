import re

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
        self.tokens = tokens  # Liste de tokens à analyser
        self.pos = 0  # Position actuelle dans la liste de tokens
        self.current_token = self.tokens[self.pos]  # Token courant, initialisé au premier token

    def advance(self):
        "Avance au token suivant dans la liste"

        self.pos += 1  # Incrémente la position actuelle
        # Met à jour le token courant, ou None si on dépasse la liste
        self.current_token = self.tokens[self.pos] if self.pos < len(self.tokens) else None

    def parse(self):
        "Parcourir et interpréter les tokens un par un"

        # Boucle principale pour traiter les tokens jusqu'à rencontrer EOF.
        while self.current_token.type != SYMBOL_TOKENS['EOF']:
            if self.current_token.type == LANG_TOKENS['DRAWCIRCLE']:
                # Si le token courant correspond à "drawCircle", appelle une méthode pour le traiter.
                self.parse_draw_circle()
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
                
            else:
                # Gère les cas où le token est inconnu ou non pris en charge.
                print(f"Unknown token: {self.current_token}")
                self.advance()  # Avance pour éviter une boucle infinie


    def parse_draw_circle(self):
        "Cette méthode lit les arguments de drawCircle(x, y, radius, color, type), vérifie leur validité grâce à des méthodes expect, et appelle une fonction associée (execute_draw_circle) avec les valeurs extraites."

        self.advance()  # Passer le token 'TT_DRAWCIRCLE' pour commencer à analyser les arguments.
        if self.current_token.type == SYMBOL_TOKENS['LPAREN']:  # Vérifie si le prochain token est '('.
            self.advance()  # Passe '('.

            "Lire les arguments : x, y, radius, color, type"

            x = self.expect_number()  # Attend un nombre pour x.
#-----------------------------------------------------------------------------
            self.expect(SYMBOL_TOKENS['COMMA'])  # Vérifie et passe ','.
#-----------------------------------------------------------------------------
            y = self.expect_number()  # Attend un nombre pour y.
#-----------------------------------------------------------------------------
            self.expect(SYMBOL_TOKENS['COMMA'])  # Vérifie et passe ','.
#-----------------------------------------------------------------------------
            radius = self.expect_number()  # Attend un nombre pour radius.
#-----------------------------------------------------------------------------
            self.expect(SYMBOL_TOKENS['COMMA'])  # Vérifie et passe ','.
#-----------------------------------------------------------------------------
            color = self.expect_color()  # Attend une couleur hexadécimale pour color.
#-----------------------------------------------------------------------------
            self.expect(SYMBOL_TOKENS['COMMA'])  # Vérifie et passe ','.
#-----------------------------------------------------------------------------
            circle_type = self.expect_string()  # Attend une chaîne ("filled" ou "empty") pour type.
#-----------------------------------------------------------------------------
            self.expect(SYMBOL_TOKENS['RPAREN'])  # Vérifie et passe ')'.
#-----------------------------------------------------------------------------

            "Exécute l'instruction drawCircle avec les arguments lus."
            self.execute_draw_circle(int(x), int(y), int(radius), color, circle_type)



    def expect(self, token_type):
        "Vérifie que le token courant est du type attendu, sinon lève une erreur"

        if self.current_token.type == token_type:  # Vérification du type du token courant
            self.advance()  # Passe au token suivant
        else:
            raise SyntaxError(f"Expected {token_type}, got {self.current_token.type}")  # Sinon, lève une erreur de syntaxe.



    def expect_number(self):
        "Vérifie que le token courant est un nombre (int ou float) et retourne sa valeur."
        
        if self.current_token.type in ['TT_INT', 'TT_FLOAT']:  # Vérifie si le token est un nombre entier ou flottant.
            value = self.current_token.value  # Récupère la valeur du token.
            self.advance()  # Passe au token suivant.
            return value  # Retourne la valeur du nombre.
        else:
            raise SyntaxError(f"Expected a number, got {self.current_token.type}")  # Lève une erreur si ce n'est pas un nombre.


    def expect_color(self):
        "Vérifie que le token courant est une couleur en hexadécimal et retourne sa valeur."
        
        if self.current_token.type == 'TT_COLOR':  # Vérifie si le token est une couleur hexadécimale.
            value = self.current_token.value  # Récupère la valeur du token.
            self.advance()  # Passe au token suivant.
            return int(value, 16)  # Convertit la couleur en entier (base 16).
        else:
            raise SyntaxError(f"Expected a color (0x...), got {self.current_token.type}")  # Lève une erreur si ce n'est pas une couleur.


    def expect_string(self):
        "Vérifie que le token courant est une chaîne et retourne sa valeur."

        if self.current_token.type == 'TT_STRING':  # Vérifie si le token est ("filled" ou "empty").
            value = self.current_token.value  # Récupère la valeur de la chaîne.
            self.advance()  # Passe au token suivant.
            return value  # Retourne la valeur de la chaîne.
        else:
            raise SyntaxError(f"Expected a string (filled/empty), got {self.current_token.type}")  # Lève une erreur si ce n'est pas le cas.


    def execute_draw_circle(self, x, y, radius, color, circle_type):
        "Exécute la fonction associée à cercleColor."
        print(f"Drawing circle at ({x}, {y}) with radius {radius}, color {hex(color)}, type {circle_type}")

        # Appelle ici une fonction SDL ou autre pour dessiner le cercle
        # Exemple d'utilisation : drawCircle(renderer, x, y, radius, color, circle_type)

#######################################
# FONCTION PRINCIPALE D'EXÉCUTION
#######################################

if __name__ == "__main__":
    code = "drawCircle(100, 200, 50, 0xFF00FF00, filled)"
    lexer = Lexer('<stdin>', code)
    tokens = lexer.make_tokens()

    print("Tokens:", tokens)

    parser = Parser(tokens)
    parser.parse()
