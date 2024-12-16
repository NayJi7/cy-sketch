# Vérifier les différentes erreurs et en ajouter d'autres
# Ajuster et ajouter les codegen pour les fonctions, opérations existantes ou ajoutées

#######################################
# IMPORTS
#######################################

def get_parser():
    import parser  # Cette importation ne sera effectuée que lorsque la fonction sera appelée.
    return parser

#######################################
# GESTION DES ERREURS
#######################################

class Error:
    """Classe de base pour toutes les erreurs."""
    def __init__(self, message):
        self.message = message

    def as_string(self):
        return f"Error: {self.message}"

class InvalidSyntaxError(Error):
    """Erreur pour une syntaxe invalide."""
    def __init__(self, ln, message):
        super().__init__(message)
        self.ln = ln

    def as_string(self):
        return f"Invalid Syntax at line {self.ln}: {self.message}"

class IllegalCharError(Error):
    """Erreur pour un caractère illégal."""
    def __init__(self, char, ln):
        super().__init__(f"Illegal character '{char}' found at line {ln}")
        self.char = char
        self.ln = ln

#######################################
# GÉNÉRATION DE CODE C INTERMÉDIAIRE
#######################################

def generate_c_code(node):
    """Génère du code C pour chaque nœud de l'AST."""

    parser = get_parser()

    if isinstance(node, parser.CursorNode):
        # Si x et y sont des flottants, gérez-les comme tels
        x_type = 'float' if isinstance(node.x, float) else 'int'
        y_type = 'float' if isinstance(node.y, float) else 'int'
        return f"{x_type} {node.name}_x = {node.x};\n{x_type} {node.name}_y = {node.y};"

    elif isinstance(node, parser.MoveNode):
        return f"move_cursor({node.cursor_name}, {node.distance});"
    
    elif isinstance(node, parser.RotateNode):
        return f"rotate_cursor({node.cursor_name}, {node.angle});"
    
    elif isinstance(node, parser.DrawCircleNode):
        # On prépare une chaîne avec les attributs du DrawNode
        return f"draw_{node.name}({node.x}, {node.y}, {node.radius}, {node.color}, {node.type});"

    elif isinstance(node, parser.IfNode):
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