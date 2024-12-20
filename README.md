# cy-sketch 

Pour le moment, Alice Franco et Jouaye Saghir travaillent sur cette partie en C en utilisant la bibliothèque SDL.
Cette partie permet de dessiner des formes dans une fenêtre à l'écran, et nous sommes actuellement en train de créer un curseur qui pourra : selectionner des formes, les deplacer, zoomer et rapetisser les formes ( cela nécessite donc de gérer les intéractions des utilisateurs). 

Actuellement, nous utilisons trois fichiers principaux :
main.c : gère la création de la fenêtre, le nettoyage , l'affichage => c'est le coeur du programme, le fichier qui relie tous les autres 
form.c : gère la création de formes ( cercle, ellipse, arc, rectangle ( avec angle droit ou angle arrondis ), polygones particuliers)  animées ou statiques et remplies ou uniquement le contour. 
De plus, il est possible de préciser la couleur et la position de la forme voulue.
clickEvents.c : gère tous les évenement utilisateurs ( clics de souris, de pavé etc)

Nous avons crée une fonction centrale, drawShape ( qu'on peut retrouver dans clickEvents.c ) qui prends les paraùètres suivants : la forme souhaitée, la position, la couleur, la taille, si il est remplie ou non et si il est animé ou non. Cette fonction oriente ensuite vers la fonction appropriée dans form.c pour générer la forme demandée.

Au départ, nous dessinions directement sur la fenêtre SDL. Cependant, lors de la création du curseur, nous avons constaté que ce dernier devait être géré sur une texture.  Travailler à la fois directement sur la fenêtre pour les formes et sur une texture pour le curseur créait donc un conflit. Nous avons donc dû reprendre nos fonction de création de formes afin de les adapter à une texture. Nous sommes donc encore en train de travailler pour adapter les codes entre eux. 
De plus, la création d'une texture nous a permis d'enlever des problèmes d'affichae "aléatoire" qu'on ne comprenait pas.
Nous avons reussi à créer un curseur, à lui donner une forme esthétique, à le déplacer, à pouvoir selectionner une forme, la déplacer, zoomer et rapeticir. Cependant, nous n'arrivons pas pour l'instant à l'implémenter sur les formes crée au préalable. Nous sommes donc en train de travailler sur cette partie la.

Pour lancer le code, il suffit de faire un make run et cela exécutera et la fenetre de dessin s'affichera.

ATTENTION : Avant de lancer le code il est important d'avoir installé au préalable la bibliothèque SDL2
