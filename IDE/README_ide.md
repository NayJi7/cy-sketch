C'est Matias VINKOVIC, qui s'occupe de cette partie IDE pour le moment.

Au début, nous voulions passer classiquement par la librairie tkinter, mais nos ambitions de design et de fonctionnalités étaient trop grandes
pour rester sur cette librairie. Depuis peu, je suis passé sur la librairie PyQt5, une bibliothèque Python permettant de créer des applications 
GUI en utilisant le framework Qt, offrant des fonctionnalités multiplateforme.

Cette dernière m'a permis d'implémenté des choses du ressort du design (colorier les mots clés, mettre les numéros de lignes) mais surtout de permettre 
l'intégration d'une console interne, permettant de récupérer les sorties du lexer/parser, et ainsi les formater et les affichers en couleurs
(rouge pour erreur, bleu pour suggestion, vert pour tout s'est bien passé : fonctionnalité encore en projet)

Afin de récupérer ces retours, je suis passé par la classe QProcess de PyQt5, permettant alors de lancer des processus externes, et de récuperer
leur retour, ainsi que leur erreur, et j'ai ainsi pu les récuperer, les disposer dans des méthodes diplay_output, de la maniere que je voulais.

Pour la partie fonctionnelle de l'ide, a savoir executer le code draw++ (DPP) mon idée fut la suivante : 
  lorsque je presse le bouton run, alors mon programme python va dans un premier temps créer un dossier (caché sous linux, visible sous windows et mac)
  dans lequel se trouvera un to_execute.dpp, il s'agit alors d'un simple save. Par la suite, la fonction que trigger le bouton run 
  lance en arriere plan (QProcess) la commande de l'interpreter/lexer/parser : interpreter.py, qui va venir chercher le fichier to_execute et ainsi 
  interpreter le fichier.

  ATTENTION : le code interpret.py, lexer.py, parser.py et my_ast.py ne sont pas les dernières versions de eux-même, les dernières modifications
  se trouvent dans la branche "compilator", il s'agit ici d'une version ancienne fonctionnelle pour tester la compatibilité avec l'IDE.
  
