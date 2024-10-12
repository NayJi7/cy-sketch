#   ,——————————————————————————————————————————————————————,
#   |                                                      |
#   |                     Header File                      |
#   |                                                      |
#   '——————————————————————————————————————————————————————'



############################################
#   CONSTANTS                              #
############################################

DIGITS = '0123456789'


############################################
#   ERRORS                                 #
############################################

class Error:
    def __init__(self, pos_start, pos_end, error_name, details):
        self.pos_start = pos_start
        self.pos_end = pos_end
        self.error_name = error_name
        self.details = details

    def as_string(self):
        result = f'In {self.pos_start.file_name} at line {self.pos_start.ln + 1}\n'
        result += f'{self.error_name}: {self.details}'
        return result
    
class IllegalCharError(Error):
    def __init__(self, pos_start, pos_end, details):
        super().__init__(pos_start, pos_end, 'Illegal Character', details)


############################################
#   POSITION                               #
############################################

class Postion:
    def __init__(self, index, ln, col, file_name, file_text):
        self.index = index
        self.ln = ln
        self.col = col
        self.file_name = file_name
        self.file_text = file_text

    def next(self, current_char):
        self.index += 1
        self.col += 1

        if current_char == '\n':
            self.ln += 1
            self.col = 0

        return self
    
    def copy(self):
        return Postion(self.index, self.ln,self.col, self.file_name, self.file_text)


############################################
#   TOKENS                                 #
############################################

TT_INT         = 'INT'
TT_FLOAT       = 'FLOAT'
TT_PLUS        = 'PLUS'
TT_MINUS       = 'MINUS'
TT_MUL         = 'MUL'
TT_DIV         = 'DIV'
TT_LPAREN      = 'LPAREN'
TT_RPAREN      = 'RPAREN'

class Token:
    def __init__(self, type, value=None):
        self.type = type
        self.value = value

    def __repr__(self):
        if self.value: return f'{self.type}:{self.value}'
        return f'{self.type}'
    

############################################
#   LEXER                                  #
############################################

class Lexer:
    def __init__(self, file_name, text):
        self.file_name = file_name
        self.text = text
        self.pos = Postion(-1, 0, -1, file_name, text)
        self.current_char = None
        self.next()
    
    def next(self):
        self.pos.next(self.current_char)
        self.current_char = self.text[self.pos.index] if self.pos.index < len(self.text) else None

    def make_tokens(self):
        tokens = []
        
        while self.current_char != None:
            if self.current_char in ' \t':      # ignoring case
                self.next()
            elif self.current_char in DIGITS:   # digits case (no need for self.next here otherwise it will skip one char)
                tokens.append(self.make_number())
            elif self.current_char == '+':      # operators cases
                tokens.append(Token(TT_PLUS))
                self.next()
            elif self.current_char == '-':
                tokens.append(Token(TT_MINUS))
                self.next()
            elif self.current_char == '*':
                tokens.append(Token(TT_MUL))
                self.next()
            elif self.current_char == '/':
                tokens.append(Token(TT_DIV))
                self.next()
            elif self.current_char == '(':
                tokens.append(Token(TT_LPAREN))
                self.next()
            elif self.current_char == ')':      
                tokens.append(Token(TT_RPAREN))
                self.next()
            else:                                # error cases
                pos_start = self.pos.copy()
                char = self.current_char
                self.next()
                return [], IllegalCharError(pos_start, self.pos,"'" + char + "'")
                    
        return tokens, None
    
    def make_number(self):
        num_str = ''
        dot_count = 0

        while self.current_char != None and self.current_char in DIGITS + '.':
            if self.current_char == '.':
                if dot_count == 1 : break
                dot_count += 1
                num_str += '.'
            else: 
                num_str += self.current_char
            self.next()

        if dot_count == 0:
            return Token(TT_INT, int(num_str))
        else:
            return Token(TT_FLOAT, float(num_str))
        

############################################
#   RUN                                    #
############################################

def run(file_name, text):
    lexer = Lexer(file_name, text)
    tokens, error = lexer.make_tokens()

    return tokens, error