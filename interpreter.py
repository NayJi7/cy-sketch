#   ,——————————————————————————————————————————————————————,
#   |                                                      |
#   |                     Exec File                        |
#   |                                                      |
#   '——————————————————————————————————————————————————————'



###############################################################
#   Interpreter to run draw++ in terminal                     #
###############################################################

import os
import lexer

text = None

while text != "exit":
    text = input('draw++ > ')
    result, error = lexer.run('<stdin>', text)

    if text=="exit": 
        restart = False
        break 
    elif text=="clear": os.system('clear')
    elif text=="restart":
        restart = True
        break
    elif error: print(error.as_string())
    else: print(result)

if restart:
    os.system('clear & python3 interpreter.py')