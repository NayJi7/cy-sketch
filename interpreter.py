#   ,——————————————————————————————————————————————————————,
#   |                                                      |
#   |                     Exec File                        |
#   |                                                      |
#   '——————————————————————————————————————————————————————'



###############################################################
#   Interpreter to run draw++ in terminal                     #
###############################################################

import os
import lexer_parser_regex as lexer_parser

text = None

while text != "exit":
    text = input('draw++ > ')
    result, error = lexer_parser.run('<stdin>', text)
    #result, error = lexer_parser_louaye.run('<stdin>', text) #drawCircle(100, 200, 50, 0xFF00FF00, filled)


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