#######################################
# CONSTANTES
#######################################

# Caractères numériques pour les nombres
DIGITS = '0123456789'

#Tokens pour les couleurs
COLORS = {
    "BLACK": 0xFF000000,      
    "WHITE": 0xFFFFFFFF,      
    "RED": 0xFFFF0000,        
    "GREEN": 0xFF00FF00,      
    "BLUE": 0xFF0000FF,       
    "YELLOW": 0xFFFFFF00,     
    "CYAN": 0xFF00FFFF,       
    "MAGENTA": 0xFFFF00FF,    
    "GRAY": 0xFF808080,       
    "SILVER": 0xFFC0C0C0,     
    "MAROON": 0xFF800000,     
    "OLIVE": 0xFF808000,      
    "LIME": 0xFFBFFF00,       
    "AQUA": 0xFF00FFFF,       
    "TURQUOISE": 0xFF40E0D0,  
    "PURPLE": 0xFF800080,     
    "INDIGO": 0xFF4B0082,     
    "GOLD": 0xFFFFD700,       
    "ORANGE": 0xFFFFA500,     
    "BROWN": 0xFFA52A2A       
}

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
    'VAR': 'TT_VAR',                # Noms de variables
    'COLOR': 'TT_COLOR',            # Noms de variables
    'STR': 'TT_STR'                 # Strings
}

# Tokens pour les symboles de syntaxe
SYMBOL_TOKENS = {
    'LPAREN': 'TT_LPAREN',   # (
    'RPAREN': 'TT_RPAREN',   # )
    'LBRACE': 'TT_LBRACE',   # {
    'RBRACE': 'TT_RBRACE',   # }
    'COMMA': 'TT_COMMA',   # ,
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
    """Représente un token avec un type et une valeur optionnelle."""
    def __init__(self, type_, value=None):
        self.type = type_
        self.value = value

    def __repr__(self):
        """Représentation textuelle d'un token, utile pour le débogage."""
        if self.value: return f'{self.type}:{self.value}'
        return f'{self.type}'