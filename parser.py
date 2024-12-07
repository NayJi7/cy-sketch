#######################################
# IMPORTS
#######################################

from tokens import *  # Contient les types de tokens et les constantes comme LANG_TOKENS['CURSOR'], LANG_TOKENS['DRAWCIRCLE'], etc.
from regex_process import *  # Contient les fonctions de traitement des expressions régulières pour le lexer

# Ces classes sont dans le même fichier ou dans des fichiers séparés
from errors_codegen import *  # Contient les classes d'erreurs, comme InvalidSyntaxError, IllegalCharError, etc.

#######################################
# PARSER
#######################################

class Parser:
    """Analyseur syntaxique qui convertit les tokens en un AST."""

    def __init__(self, tokens):
        self.tokens = tokens
        self.idx = 0  # Pointeur pour parcourir la liste des tokens
        self.ln = 1  # Initialiser la ligne à 1

    def current_token(self):
        """Retourne le token actuel."""
        if self.idx < len(self.tokens):
            return self.tokens[self.idx]
        return None  # Si on dépasse la liste des tokens

    def consume(self):
        """Avance au token suivant et incrémente la ligne si c'est un token EOL."""
        token = self.current_token()
        if token and token.type == SYMBOL_TOKENS['EOL']:
            self.ln += 1  # Incrémenter la ligne à chaque fin de ligne
        self.idx += 1

    def parse(self):
        """Point d'entrée pour l'analyse."""
        statements = []
        while self.current_token().type != SYMBOL_TOKENS['EOF']:
            stmt, error = self.parse_statement()
            if error:
                return None, error  # Retourner l'erreur si présente
            if stmt:
                statements.append(stmt)
        return statements, None

    def parse_statement(self):
        """Parse une déclaration (par exemple, un curseur ou une instruction de dessin)."""
        token = self.current_token()

        if token.type == SYMBOL_TOKENS['EOL']:
            self.consume()  # Consommer le token EOL sans l'ajouter à l'AST
            return None, None  # Pas de nœud à ajouter, mais on passe à la ligne suivante
        
        elif token.type == LANG_TOKENS['CURSOR']:
            return self.parse_cursor()

        elif token.type == LANG_TOKENS['DRAWCIRCLE']:
            return self.parse_drawcircle()

        elif token.type == LANG_TOKENS['MOVE']:
            return self.parse_move()

        elif token.type == LANG_TOKENS['ROTATE']:
            return self.parse_rotate()

        else:
            error = InvalidSyntaxError(self.ln, f"Unexpected token {token.value}")
            return None, error  # Retourner l'erreur

    def parse_cursor(self):
        """Parse une déclaration de curseur."""
        self.consume()  # Consommer le token CURSOR
        cursor_name = self.current_token()
        if cursor_name.type != USER_TOKENS['IDENTIFIER']:
            error = InvalidSyntaxError(cursor_name.ln, f"Expected an identifier for cursor, got {cursor_name.type}")
            return None, error  # Retourner l'erreur
        self.consume()  # Consommer le nom du curseur

        # Expecting parentheses with coordinates (x, y)
        if self.current_token().type == SYMBOL_TOKENS['LPAREN']:
            self.consume()  # Consommer '('
            x = self.current_token()
            if x.type != USER_TOKENS['INT'] and x.type != USER_TOKENS['FLOAT']:
                error = InvalidSyntaxError(x.ln, f"Expected an integer or float for x, got {x.type}")
                return None, error  # Retourner l'erreur
            self.consume()  # Consommer x

            y = self.current_token()
            if y.type != USER_TOKENS['INT'] and y.type != USER_TOKENS['FLOAT']:
                error = InvalidSyntaxError(y.ln, f"Expected an integer or float for y, got {y.type}")
                return None, error  # Retourner l'erreur
            self.consume()  # Consommer y

            if self.current_token().type != SYMBOL_TOKENS['RPAREN']:
                error = InvalidSyntaxError(self.ln, "Expected closing parenthesis ')' after coordinates")
                return None, error  # Retourner l'erreur
            self.consume()  # Consommer ')'

            return CursorNode(cursor_name.value, x.value, y.value), None  # Retourner le nœud

        error = InvalidSyntaxError(self.ln, "Expected '(' after cursor name")
        return None, error  # Retourner l'erreur

    def parse_drawcircle(self):
        """Parse une instruction de dessin de cercle."""
        self.consume()  # Consommer le token DRAWCIRCLE
        name = self.current_token()
        if name.type != USER_TOKENS['IDENTIFIER']:
            raise InvalidSyntaxError(self.idx, f"Expected an identifier for shape name, got {name.type}")
        self.consume()  # Consommer le nom de la forme

        args = []
        while self.current_token() and self.current_token().type not in [SYMBOL_TOKENS['EOL'], SYMBOL_TOKENS['EOF']]:
            arg = self.current_token()
            if arg.type in [USER_TOKENS['INT'], USER_TOKENS['FLOAT'], USER_TOKENS['VAR'], LANG_TOKENS['COLOR'], USER_TOKENS['STR']]:
                args.append(arg.value)  # Ajouter l'argument
            else:
                raise InvalidSyntaxError(self.idx, f"Unexpected argument type: {arg.type}")
            self.consume()

        # Ajouter la gestion du EOL ou EOF ici pour s'assurer que la fin de l'instruction est correctement traitée
        if self.current_token() and self.current_token().type == SYMBOL_TOKENS['EOL']:
            self.consume()  # Consommer le EOL

        # S'assurer qu'il y a bien 5 arguments avant de créer le nœud DrawCircleNode
        if len(args) == 5:
            return DrawCircleNode(name.value, args[0], args[1], args[2], args[3], args[4]), None  # Retourner le nœud DrawCircleNode
        else:
            raise InvalidSyntaxError(self.idx, "Expected 5 arguments for drawcircle")


    def parse_move(self):
        """Parse une instruction de déplacement d'un curseur."""
        self.consume()  # Consommer le token MOVE
        cursor_name = self.current_token()
        if cursor_name.type != USER_TOKENS['IDENTIFIER']:
            error = InvalidSyntaxError(cursor_name.ln, f"Expected an identifier for cursor name, got {cursor_name.type}")
            return None, error  # Retourner l'erreur
        self.consume()  # Consommer le nom du curseur

        # Expecting a distance argument
        distance = self.current_token()
        if distance.type != USER_TOKENS['INT'] and distance.type != USER_TOKENS['FLOAT']:
            error = InvalidSyntaxError(distance.ln, f"Expected an integer or float for distance, got {distance.type}")
            return None, error  # Retourner l'erreur
        self.consume()  # Consommer la distance

        return MoveNode(cursor_name.value, distance.value), None  # Retourner le nœud

    def parse_rotate(self):
        """Parse une instruction de rotation d'un curseur."""
        self.consume()  # Consommer le token ROTATE
        cursor_name = self.current_token()
        if cursor_name.type != USER_TOKENS['IDENTIFIER']:
            error = InvalidSyntaxError(cursor_name.ln, f"Expected an identifier for cursor name, got {cursor_name.type}")
            return None, error  # Retourner l'erreur
        self.consume()  # Consommer le nom du curseur

        # Expecting an angle argument
        angle = self.current_token()
        if angle.type != USER_TOKENS['INT'] and angle.type != USER_TOKENS['FLOAT']:
            error = InvalidSyntaxError(angle.ln, f"Expected an integer or float for angle, got {angle.type}")
            return None, error  # Retourner l'erreur
        self.consume()  # Consommer l'angle

        return RotateNode(cursor_name.value, angle.value), None  # Retourner le noeud

#######################################
# AST NODES 
#######################################

# Ces classes représentent différentes instructions de dpp sous forme de nœuds dans l'AST

class CursorNode:
    """Nœud AST pour créer un curseur."""
    def __init__(self, name, x, y):
        self.name = name
        self.x = x
        self.y = y

    def __repr__(self):
        return f'Cursor{self.name}({self.x}, {self.y})'

class MoveNode:
    """Nœud AST pour déplacer un curseur."""
    def __init__(self, cursor_name, distance):
        self.cursor_name = cursor_name
        self.distance = distance

    def __repr__(self):
        return f'Move({self.cursor_name}, {self.distance})'

class RotateNode:
    """Nœud AST pour faire pivoter un curseur."""
    def __init__(self, cursor_name, angle):
        self.cursor_name = cursor_name
        self.angle = angle

    def __repr__(self):
        return f'Rotate({self.cursor_name}, {self.angle})'

class DrawCircleNode:
    """Nœud AST pour dessiner une forme."""
    def __init__(self, name, x, y, radius, color, tp):
        self.name = name
        self.x = x
        self.y = y
        self.radius = radius
        self.color = color
        self.type = tp

    def __repr__(self):
        return f'Draw{self.name}({self.x}, {self.y}, {self.radius}, {self.color}, {self.type})'

class IfNode:
    """Nœud AST pour une condition IF."""
    def __init__(self, condition, true_branch, false_branch=None):
        self.condition = condition
        self.true_branch = true_branch
        self.false_branch = false_branch

    def __repr__(self):
        return f'If({self.condition}, {self.true_branch}, Else: {self.false_branch})'