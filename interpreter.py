#   ,——————————————————————————————————————————————————————,
#   |                                                      |
#   |                     Exec File                        |
#   |                                                      |
#   '——————————————————————————————————————————————————————'



###############################################################
#   Interpreter to run draw++ in terminal                     #
###############################################################

import os
import lexer_parser2 as lexer_parser

text = None

while text != "exit":
    text = input('draw++ > ')
    result, error = lexer_parser.run('<stdin>', text)

    if text=="exit": 
        restart = False
        break 
    elif text=="clear": os.system('clear')
    elif text=="cls": os.system('cls')
    elif text=="restart":
        restart = True
        break
    elif error: print(error.as_string())
    else: print(result)

if restart:
    os.system('clear & python3 interpreter.py')