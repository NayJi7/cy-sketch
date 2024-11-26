# ,——————————————————————————————————————————————————————,
# |                                                      |
# |                    DRAW++ COMPILER                   |
# |                                                      |
# '——————————————————————————————————————————————————————'

############### PLAN ###############
 
# 1 - ERREURS : gestion des erreurs
# 2 - LEXER : Transforme le texte en une liste de tokens reconnaissables par le langage draw++ (analyse lexicale)
# 3 - PARSER : Analyse les tokens pour créer l'AST (analyse syntaxique)
# 4 - AST : Représentation structurée des instructions dpp (arbre syntaxique abstrait)
# 5 - génération de code C :  Traduit l'AST en code C intermédiaire
# 6 - exécution principale

#####################################


#######################################
# IMPORTS
#######################################

# Bibliothèque pour afficher les erreurs avec des flèches dans le code source
from strings_with_arrows import *
from tokens import *
from regex_process import *


#######################################
# GESTION DES ERREURS
#######################################

class Error:
    """Classe de base pour gérer les erreurs dans le code."""
    def __init__(self, pos_start, pos_end, error_name, details):
        self.pos_start = pos_start
        self.pos_end = pos_end
        self.error_name = error_name
        self.details = details
    
    def as_string(self):
        """Retourne une chaîne de texte décrivant l'erreur avec le contexte du code."""
        result  = f'{self.error_name}: {self.details}\n'
        return result

class IllegalCharError(Error):
    """Erreur pour un caractère illégal dans le code."""
    def __init__(self, pos_start, pos_end, details):
        super().__init__(pos_start, pos_end, 'Illegal Character', details)

class InvalidSyntaxError(Error):
    """Erreur pour une syntaxe incorrecte."""
    def __init__(self, pos_start, pos_end, details=''):
        super().__init__(pos_start, pos_end, 'Invalid Syntax', details)

#######################################
# LEXER
#######################################

class Lexer:
    """Analyseur lexical qui convertit le texte source en une liste de tokens."""
    
    def __init__(self, fn, text):
        self.fn = fn
        self.ln = 0
        self.text = text
    
    def make_tokens(self):
        """Convertit tout le texte source en une liste de tokens utilisables par le parser."""
        tokens = []
        result = 0

        result = cursor(self.text, tokens, result)
        result = drawcircle(self.text, tokens, result)

        if(result == 0) :
            operations(self.text, tokens)

        return tokens, None

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
        return f'Cursor({self.name}, {self.x}, {self.y})'

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

class DrawNode:
    """Nœud AST pour dessiner une forme."""
    def __init__(self, shape, *args):
        self.shape = shape
        self.args = args

    def __repr__(self):
        return f'Draw({self.shape}, {self.args})'

class IfNode:
    """Nœud AST pour une condition IF."""
    def __init__(self, condition, true_branch, false_branch=None):
        self.condition = condition
        self.true_branch = true_branch
        self.false_branch = false_branch

    def __repr__(self):
        return f'If({self.condition}, {self.true_branch}, Else: {self.false_branch})'

#######################################
# GÉNÉRATION DE CODE C INTERMÉDIAIRE
#######################################

def generate_c_code(node):
    """Génère du code C pour chaque nœud de l'AST."""
    if isinstance(node, CursorNode):
        return f"Cursor {node.name} = create_cursor({node.x}, {node.y});"
    
    elif isinstance(node, MoveNode):
        return f"move_cursor({node.cursor_name}, {node.distance});"
    
    elif isinstance(node, RotateNode):
        return f"rotate_cursor({node.cursor_name}, {node.angle});"
    
    elif isinstance(node, DrawNode):
        args = ', '.join(map(str, node.args))
        return f"draw_{node.shape}({args});"
    
    elif isinstance(node, IfNode):
        true_branch_code = '\n'.join([generate_c_code(stmt) for stmt in node.true_branch])
        false_branch_code = '\n'.join([generate_c_code(stmt) for stmt in node.false_branch]) if node.false_branch else ""
        return f"if ({generate_c_code(node.condition)}) {{\n{true_branch_code}\n}} else {{\n{false_branch_code}\n}}"

    else:
        raise Exception(f"Unrecognized AST Node: {node}")

def compile_to_c(ast):
    """Compile l'AST complet en code C."""
    c_code = []
    for node in ast:
        c_code.append(generate_c_code(node))
    return '\n'.join(c_code)

#######################################
# FONCTION PRINCIPALE D'EXÉCUTION
#######################################

def run(fn, text):
    """Fonction principale pour le lexer, parser et générateur de code C."""
    
    lexer = Lexer(fn, text)
    tokens, error = lexer.make_tokens()
    if error: return None, error

    parser = Parser(tokens)
    ast = parser.parse()
    if ast.error: return None, ast.error

    c_code = compile_to_c(ast.node)
    return c_code, None
