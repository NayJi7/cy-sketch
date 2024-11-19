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
    'DRAW': 'TT_DRAW',      # Dessiner une forme
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
        result += f'File {self.pos_start.fn}, line {self.pos_start.ln + 1}'
        result += '\n\n' + string_with_arrows(self.pos_start.ftxt, self.pos_start, self.pos_end)
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
# POSITION
#######################################

class Position:
    """Classe pour suivre la position dans le texte source, utilisée pour les erreurs."""
    def __init__(self, idx, ln, col, fn, ftxt):
        self.idx = idx  # Index du caractère actuel
        self.ln = ln    # Ligne actuelle
        self.col = col  # Colonne actuelle
        self.fn = fn    # Nom du fichier
        self.ftxt = ftxt  # Contenu du fichier

    def next(self, current_char=None):
        """Avance d'un caractère dans le texte source."""
        self.idx += 1
        self.col += 1

        if current_char == '\n':  # Si le caractère est une fin de ligne
            self.ln += 1
            self.col = 0

        return self

    def copy(self):
        """Retourne une copie de la position actuelle."""
        return Position(self.idx, self.ln, self.col, self.fn, self.ftxt)

#######################################
# TOKEN
#######################################

class Token:
    """Représente un token avec un type et une valeur optionnelle."""
    def __init__(self, type_, value=None, pos_start=None, pos_end=None):
        self.type = type_
        self.value = value
        if pos_start is not None:
            self.pos_start = pos_start
        if pos_end is not None:
            self.pos_end = pos_end

    def __repr__(self):
        """Représentation textuelle d'un token, utile pour le débogage."""
        if self.value: return f'{self.type}:{self.value}'
        return f'{self.type}'

#######################################
# LEXER
#######################################

class Lexer:
    """Analyseur lexical qui convertit le texte source en une liste de tokens."""
    
    def __init__(self, fn, text):
        self.fn = fn
        self.text = text
        self.pos = Position(-1, 0, -1, fn, text)
        self.current_char = None
        self.next()
    
    def next(self):
        """Avance au caractère suivant dans le texte source."""
        self.pos.next(self.current_char)
        self.current_char = self.text[self.pos.idx] if self.pos.idx < len(self.text) else None
    
    def make_tokens(self):
        """Convertit tout le texte source en une liste de tokens utilisables par le parser."""
        tokens = []

        pattern_cursor = r"(CURSOR)\s+([A-Za-z0-9]+)\((\d+\.\d+|\d+|[A-Za-z]+),\s*(\d+\.\d+|\d+|[A-Za-z]+)\)"
        match = re.search(pattern_cursor, self.text)

        if match:
            # Vérifie si l'identifiant est un mot-clé
            type_ = LANG_TOKENS.get(match.group(1), USER_TOKENS['IDENTIFIER'])
            tokens.append(Token(type_, match.group(1), match.start(1), match.end(1)))

            # Vérifie si l'identifiant est un mot-clé
            type_ = LANG_TOKENS.get(match.group(2), USER_TOKENS['IDENTIFIER'])
            tokens.append(Token(type_, match.group(2), match.start(2), match.end(2)))

            count = 0
            try:
                # Tentative de conversion en entier
                int(match.group(3))
            except ValueError:
                count +=1
                pass

            if count == 0:
                tokens.append(Token(USER_TOKENS['INT'], int(match.group(3)), match.start(3), match.end(3)))

            try:
                # Si la conversion en int échoue, tenter en float
                if count != 0:
                    float(match.group(3))
                else :
                    count=3 # is an int and has been append
            except ValueError:
                count += 1
                pass

            if count == 1:
                tokens.append(Token(USER_TOKENS['FLOAT'], float(match.group(3)), match.start(3), match.end(3)))
                
            if count == 2:
                # Si les deux conversions échouent, alors c'est une chaîne
                tokens.append(Token(USER_TOKENS['VAR'], match.group(3), match.start(3), match.end(3)))


            count = 0
            try:
                # Tentative de conversion en entier
                int(match.group(4))
            except ValueError:
                count +=1
                pass

            if count == 0:
                tokens.append(Token(USER_TOKENS['INT'], int(match.group(4)), match.start(4), match.end(4)))

            try:
                # Si la conversion en int échoue, tenter en float
                if count != 0:
                    float(match.group(4))
                else :
                    count=3 # is an int and has been append
            except ValueError:
                count += 1
                pass

            if count == 1:
                tokens.append(Token(USER_TOKENS['FLOAT'], float(match.group(4)), match.start(4), match.end(4)))
                
            if count == 2:
                # Si les deux conversions échouent, alors c'est une chaîne
                tokens.append(Token(USER_TOKENS['VAR'], match.group(4), match.start(4), match.end(4)))

            tokens.append(Token(SYMBOL_TOKENS['EOF'], None, match.end(), match.end()))

        pattern_operation = r"([A-Za-z0-9]+(?:\.[A-Za-z0-9]+)?)\s*([\+\-\*\/])\s*([A-Za-z0-9]+(?:\.[A-Za-z0-9]+)?)"
        match_op= re.search(pattern_operation, self.text)

        if match_op:
            try:
                # Si la conversion en int fonctionne, c'est un entier
                tokens.append(Token(USER_TOKENS['INT'], int(match_op.group(1)), match_op.start(1), match_op.end(1)))
            except ValueError:
                try:
                    # Si la conversion en float fonctionne, c'est un flottant
                    tokens.append(Token(USER_TOKENS['FLOAT'], float(match_op.group(1)), match_op.start(1), match_op.end(1)))
                except ValueError:
                    # Sinon, c'est une chaîne
                    tokens.append(Token(USER_TOKENS['VAR'], match_op.group(1), match_op.start(1), match_op.end(1)))

            tokens.append(Token(OPERATORS_TOKENS[match_op.group(2)], None, match_op.start(2), match_op.end(2)))

            try:
                # Si la conversion en int fonctionne, c'est un entier
                tokens.append(Token(USER_TOKENS['INT'], int(match_op.group(3)), match_op.start(3), match_op.end(3)))
            except ValueError:
                try:
                    # Si la conversion en float fonctionne, c'est un flottant
                    tokens.append(Token(USER_TOKENS['FLOAT'], float(match_op.group(3)), match_op.start(3), match_op.end(3)))
                except ValueError:
                    # Sinon, c'est une chaîne
                    tokens.append(Token(USER_TOKENS['VAR'], match_op.group(3), match_op.start(3), match_op.end(3)))
            
                tokens.append(Token(SYMBOL_TOKENS['EOF'], None, match_op.end(), match_op.end()))

        # while self.current_char is not None:
        #     if self.current_char in ' \t':  # Ignore les espaces et tabulations
        #         self.next()
        #     elif self.current_char in DIGITS:
        #         tokens.append(self.make_number())
        #     elif self.current_char.isalpha():
        #         tokens.append(self.make_identifier_or_keyword())
        #     elif self.current_char == '{':
        #         tokens.append(Token(SYMBOL_TOKENS['LBRACE'], pos_start=self.pos))
        #         self.next()
        #     elif self.current_char == '}':
        #         tokens.append(Token(SYMBOL_TOKENS['RBRACE'], pos_start=self.pos))
        #         self.next()
        #     elif self.current_char == ')':
        #         tokens.append(Token(SYMBOL_TOKENS['RPAREN'], pos_start=self.pos))
        #         self.next()
        #     elif self.current_char == '(':
        #         tokens.append(Token(SYMBOL_TOKENS['LPAREN'], pos_start=self.pos))
        #         self.next()
        #     elif self.current_char == ',':
        #         tokens.append(Token(SYMBOL_TOKENS['COMMA'], pos_start=self.pos))
        #         self.next()
        #     else:
        #         pos_start = self.pos.copy()
        #         char = self.current_char
        #         self.next()
        #         return [], IllegalCharError(pos_start, self.pos, f"'{char}'")


        return tokens, None

    def make_identifier_or_keyword(self):
        """Identifie un mot-clé ou un identifiant de l'utilisateur."""
        id_str = ''
        pos_start = self.pos.copy()
        
        while self.current_char is not None and (self.current_char.isalnum() or self.current_char == '_'):
            id_str += self.current_char
            self.next()
        
        # Vérifie si l'identifiant est un mot-clé
        type_ = LANG_TOKENS.get(id_str, USER_TOKENS['IDENTIFIER'])
        
        return Token(type_, id_str, pos_start, self.pos)
    
    def make_number(self):
        """Crée un token pour un nombre (entier ou flottant)."""
        num_str = ''
        dot_count = 0
        pos_start = self.pos.copy()

        while self.current_char is not None and (self.current_char.isdigit() or self.current_char == '.'):
            if self.current_char == '.':
                if dot_count == 1: break  # Un seul point autorisé
                dot_count += 1
            num_str += self.current_char
            self.next()
        
        # Retourne un token entier ou flottant selon le nombre de points
        if dot_count == 0:
            return Token(USER_TOKENS['INT'], int(num_str), pos_start, self.pos)
        else:
            return Token(USER_TOKENS['FLOAT'], float(num_str), pos_start, self.pos)

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
# PARSE RESULT
#######################################

class ParseResult:
    """Contient le résultat d'une analyse syntaxique, incluant les erreurs éventuelles."""
    def __init__(self):
        self.error = None
        self.node = None

    def register(self, res):
        """Enregistre un résultat, récupère le nœud si succès."""
        if isinstance(res, ParseResult):
            if res.error: self.error = res.error
            return res.node
        return res

    def success(self, node):
        """Indique une réussite de parsing avec un nœud."""
        self.node = node
        return self

    def failure(self, error):
        """Indique un échec de parsing avec une erreur."""
        self.error = error
        return self

#######################################
# PARSER
#######################################

class Parser:
    """Analyseur syntaxique pour construire un AST à partir des tokens draw++."""
    
    def __init__(self, tokens):
        self.tokens = tokens
        self.tok_idx = -1
        self.next()

    def next(self):
        """Avance au token suivant."""
        self.tok_idx += 1
        self.current_tok = self.tokens[self.tok_idx] if self.tok_idx < len(self.tokens) else None
        return self.current_tok

    def parse(self):
        """Analyse une liste d'instructions et génère l'AST complet."""
        res = ParseResult()
        instructions = []

        while self.current_tok.type != SYMBOL_TOKENS['EOF']:
            instr = self.parse_instruction()
            if instr.error: 
                return instr
            instructions.append(instr.node)
            self.next()

        return res.success(instructions)

    def parse_instruction(self):
        """Identifie et analyse une instruction spécifique."""
        tok = self.current_tok
        
        if tok.type == LANG_TOKENS['CURSOR']:
            return self.parse_cursor()
        elif tok.type == LANG_TOKENS['MOVE']:
            return self.parse_move()
        elif tok.type == LANG_TOKENS['ROTATE']:
            return self.parse_rotate()
        elif tok.type == LANG_TOKENS['DRAW']:
            return self.parse_draw()
        elif tok.type == LANG_TOKENS['IF']:
            return self.parse_if()

        return ParseResult().failure(InvalidSyntaxError(tok.pos_start, tok.pos_end, "Instruction invalide"))

    def parse_cursor(self):
        """Analyse l'instruction de création d'un curseur."""
        res = ParseResult()
        self.next()

        if self.current_tok.type != USER_TOKENS['IDENTIFIER']:
            return res.failure(InvalidSyntaxError(self.current_tok.pos_start, self.current_tok.pos_end, "Nom de curseur attendu"))

        cursor_name = self.current_tok.value
        self.next()
        
        if self.current_tok.type != SYMBOL_TOKENS['LPAREN']:
            return res.failure(InvalidSyntaxError(self.current_tok.pos_start, self.current_tok.pos_end, "'(' attendu"))
        
        if(self.next() == USER_TOKENS['INT'] or USER_TOKENS['FLOAT']):
            x = self.current_tok.value
        
        self.next()
        if(self.current_tok.type != SYMBOL_TOKENS['COMMA']):
            return res.failure(InvalidSyntaxError(self.current_tok.pos_start, self.current_tok.pos_end, "',' attendu"))

        if(self.next() == USER_TOKENS['INT'] or USER_TOKENS['FLOAT']):
            y = self.current_tok.value
            
        self.next()
        
        return res.success(CursorNode(cursor_name, x, y))

    def parse_move(self):
        """Analyse l'instruction MOVE pour déplacer un curseur."""
        res = ParseResult()
        self.next()
        
        if self.current_tok.type != USER_TOKENS['IDENTIFIER']:
            return res.failure(InvalidSyntaxError(self.current_tok.pos_start, self.current_tok.pos_end, "Nom de curseur attendu"))
        
        cursor_name = self.current_tok.value
        self.next()
        
        if self.current_tok.type != SYMBOL_TOKENS['LPAREN']:
            return res.failure(InvalidSyntaxError(self.current_tok.pos_start, self.current_tok.pos_end, "'(' attendu"))
        
        self.next()
        distance = self.current_tok.value
        self.next()  
        
        if self.current_tok.type != SYMBOL_TOKENS['RPAREN']:
            return res.failure(InvalidSyntaxError(self.current_tok.pos_start, self.current_tok.pos_end, "')' attendu"))
        
        return res.success(MoveNode(cursor_name, distance))

    def parse_rotate(self):
        """Analyse l'instruction ROTATE pour faire pivoter un curseur."""
        res = ParseResult()
        self.next()
        
        if self.current_tok.type != USER_TOKENS['IDENTIFIER']:
            return res.failure(InvalidSyntaxError(self.current_tok.pos_start, self.current_tok.pos_end, "Nom de curseur attendu"))
        
        cursor_name = self.current_tok.value
        self.next()
        
        if self.current_tok.type != SYMBOL_TOKENS['LPAREN']:
            return res.failure(InvalidSyntaxError(self.current_tok.pos_start, self.current_tok.pos_end, "'(' attendu"))
        
        self.next()
        angle = self.current_tok.value
        self.next()
        
        if self.current_tok.type != SYMBOL_TOKENS['RPAREN']:
            return res.failure(InvalidSyntaxError(self.current_tok.pos_start, self.current_tok.pos_end, "')' attendu"))
        
        return res.success(RotateNode(cursor_name, angle))

    def parse_draw(self):
        """Analyse l'instruction DRAW pour dessiner une forme."""
        res = ParseResult()
        self.next()
        
        if self.current_tok.type != USER_TOKENS['IDENTIFIER']:
            return res.failure(InvalidSyntaxError(self.current_tok.pos_start, self.current_tok.pos_end, "Nom de forme attendu"))
        
        shape = self.current_tok.value
        self.next()
        
        args = []
        
        if self.current_tok.type != SYMBOL_TOKENS['LPAREN']:
            return res.failure(InvalidSyntaxError(self.current_tok.pos_start, self.current_tok.pos_end, "'(' attendu"))
        
        self.next()
        while self.current_tok.type != SYMBOL_TOKENS['RPAREN']:
            args.append(self.current_tok.value)
            self.next()
        
        return res.success(DrawNode(shape, *args))

    def parse_if(self):
        """Analyse une instruction IF pour une condition."""
        res = ParseResult()
        self.next()

        condition = res.register(self.expression())
        if res.error: return res

        if self.current_tok.type != SYMBOL_TOKENS['LBRACE']:
            return res.failure(InvalidSyntaxError(self.current_tok.pos_start, self.current_tok.pos_end, "'{' attendu après IF"))
        
        self.next()
        true_branch = res.register(self.statements())
        if res.error: return res

        false_branch = None
        if self.current_tok.type == LANG_TOKENS['ELSE']:
            self.next()
            if self.current_tok.type != SYMBOL_TOKENS['LBRACE']:
                return res.failure(InvalidSyntaxError(self.current_tok.pos_start, self.current_tok.pos_end, "'{' attendu après ELSE"))
            self.next()
            false_branch = res.register(self.statements())
            if res.error: return res

        return res.success(IfNode(condition, true_branch, false_branch))

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
