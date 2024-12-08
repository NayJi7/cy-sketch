
# parsetab.py
# This file is automatically generated. Do not edit.
# pylint: disable=W,C,R
_tabversion = '3.10'

_lr_method = 'LALR'

_lr_signature = 'CIRCLE LINE LOUAYE LPAREN NUMBER RPARENcommand : LINE LPAREN NUMBER NUMBER RPARENcommand : CIRCLE NUMBERcommand : LOUAYE NUMBER LINE'
    
_lr_action_items = {'LINE':([0,7,],[2,9,]),'CIRCLE':([0,],[3,]),'LOUAYE':([0,],[4,]),'$end':([1,6,9,11,],[0,-2,-3,-1,]),'LPAREN':([2,],[5,]),'NUMBER':([3,4,5,8,],[6,7,8,10,]),'RPAREN':([10,],[11,]),}

_lr_action = {}
for _k, _v in _lr_action_items.items():
   for _x,_y in zip(_v[0],_v[1]):
      if not _x in _lr_action:  _lr_action[_x] = {}
      _lr_action[_x][_k] = _y
del _lr_action_items

_lr_goto_items = {'command':([0,],[1,]),}

_lr_goto = {}
for _k, _v in _lr_goto_items.items():
   for _x, _y in zip(_v[0], _v[1]):
       if not _x in _lr_goto: _lr_goto[_x] = {}
       _lr_goto[_x][_k] = _y
del _lr_goto_items
_lr_productions = [
  ("S' -> command","S'",1,None,None,None),
  ('command -> LINE LPAREN NUMBER NUMBER RPAREN','command',5,'p_command_line','test.py',44),
  ('command -> CIRCLE NUMBER','command',2,'p_command_circle','test.py',48),
  ('command -> LOUAYE NUMBER LINE','command',3,'p_command_louaye','test.py',52),
]
