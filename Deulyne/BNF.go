// Le lien entre BNF (Backus-Naur Form) et le parser :
// - La BNF est une manière formelle de décrire la grammaire d'un langage. 
// - Chaque règle BNF est implémentée ici comme une fonction avec p[0], p[1], ... 
//   pour représenter les différents éléments de la règle.
//
// Exemple : 
// En BNF : <instruction> ::= "draw" <forme> "(" <paramètres> ")"
// Ici, la règle est implémentée pour construire une structure syntaxique.
// p[1] contient "draw", p[2] la forme, p[4] les paramètres, et p[0] est la structure finale.
//
// Cela permet au parser d'utiliser les règles pour valider et organiser le code source.

<programme> ::= <instruction> | <programme> <instruction>

<instruction> ::= <dessin> | <déplacement> | <rotation> | <couleur> | <assignation> | <animation> | <conditionnelle> | <boucle> | "{" <programme> "}"

<dessin> ::= "draw" <forme> "(" <paramètres> ")"
<forme> ::= "line" | "circle" | "square" | "arc" | "point"
<paramètres> ::= <nombre> ("," <nombre>)*

<déplacement> ::= "move" "(" <nombre> "," <nombre> ")"
<rotation> ::= "rotate" "(" <nombre> ")"
<couleur> ::= "color" "(" <valeur_couleur> ")"
<valeur_couleur> ::= <chaine>
<assignation> ::= "variable" <identifiant> "=" <valeur>
<valeur> ::= <nombre> | <chaine>
<animation> ::= "animate" "(" <paramètres_animation> ")"
<paramètres_animation> ::= <nombre> ("," <nombre>)*

<conditionnelle> ::= "if" "(" <expression_logique> ")" "{" <programme> "}" [ "else" "{" <programme> "}" ]
<expression_logique> ::= <valeur> <opérateur_comparaison> <valeur>
<opérateur_comparaison> ::= "==" | "!=" | "<" | ">" | "<=" | ">="
<boucle> ::= "for" "(" <assignation> ";" <expression_logique> ";" <assignation> ")" "{" <programme> "}"

<nombre> ::= <chiffre> | <chiffre><nombre>
<chiffre> ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"
<chaine> ::= "\"" <texte> "\""
<texte> ::= <caractere> | <caractere><texte>
<caractere> ::= nimporte quel caractère sauf "\""
<identifiant> ::= <lettre> | <lettre><identifiant>
<lettre> ::= "a" ... "z" | "A" ... "Z" | "_"
