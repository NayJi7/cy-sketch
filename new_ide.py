from PyQt5.QtWidgets import (QApplication, QMainWindow, QTextEdit, QVBoxLayout, QPushButton, QFileDialog, QMessageBox, QWidget, QMenuBar, QAction, QToolBar, QPlainTextEdit)
from PyQt5.QtGui import QTextCharFormat, QColor, QSyntaxHighlighter, QTextCursor, QFont, QIcon
from PyQt5.QtCore import Qt, QProcess
import os
import platform
import subprocess

class SyntaxHighlighter(QSyntaxHighlighter):
    ''' Cette classe s'occupe de la coloration syntaxique'''

    ''' Elle hérite la classe QSyntaxHighlighter qui est une classe native de PyQt5, une classe faite pour mettre en couleur
                        des textes dans des zones de texte'''
    def __init__(self, document):
        super().__init__(document) #j'hérite de la superclass QSyntaxHighlighter
        self.rules = [] #ensemble des règles de coloration

        # Coloration des mots-clés dpp (hors fonctions)
        keyword_format = QTextCharFormat()
        keyword_format.setForeground(QColor("#007ACC"))  # Bleu moderne
        keyword_format.setFontWeight(QFont.Bold)
        keywords = ["if", "else", "for", "while", "return", "def", "class", "var"]
        self.add_rules(keywords, keyword_format)
        #coloration des fonctions dpp
        keyword_format2 = QTextCharFormat()
        keyword_format2.setForeground(QColor("#ff5733"))
        keyword_format2.setFontWeight(QFont.Bold)
        functions_word1 = ["rotate", "cursor","draw line","draw circle"]
        self.add_rules(functions_word1,keyword_format2)
        #coloration des modules d'importation
        keyword_format3 = QTextCharFormat()
        keyword_format3.setForeground(QColor("#e873f3"))
        keyword_format3.setFontWeight(QFont.Bold)
        functions_word2 = ["import", "from"]
        self.add_rules(functions_word2,keyword_format3)
        # Coloration des parenthèses
        parenthesis_format = QTextCharFormat()
        parenthesis_format.setForeground(QColor("#FFD700"))  # Jaune doré
        parentheses = [r"\(", r"\)", r"\{", r"\}", r"\[", r"\]"]
        self.add_rules(parentheses, parenthesis_format)


    def add_rules(self, patterns, text_format):
        '''méthode pour ajouter une règle de coloration'''
        for pattern in patterns:
            regex = f"\\b{pattern}\\b"
            self.rules.append((regex, text_format))

    def highlightBlock(self, text):
        '''Applique les règles de coloration sur une ligne de texte.'''
        for regex, text_format in self.rules:  # Parcourt chaque règle (regex, format)
            position = 0  # Initialise la position de départ
            while True:
                match = self.findMatch(regex, text, position)  # Recherche une correspondance dans le texte
                if match:  # Si une correspondance est trouvée
                    start, length = match  # Récupère la position de départ et la longueur
                    self.setFormat(start, length, text_format)  # Applique le format à la portion correspondante
                    position = start + length  # Avance pour chercher la prochaine correspondance
                else:  # Si aucune correspondance n'est trouvée, arrête la boucle
                    
                    break


    def findMatch(self, regex, text, start):
        '''Recherche une correspondance pour un motif regex dans le texte à partir d'une position donnée.'''
        import re  # Importe le module des expressions régulières
        match = re.search(regex, text[start:])  # Cherche le motif regex dans le texte à partir de la position donnée
        if match:  # Si une correspondance est trouvée
            return start + match.start(), match.end() - match.start()  # Retourne la position de départ et la longueur
        return None  # Si aucune correspondance, retourne None


class MyDrawppIDE(QMainWindow):
    '''Classe principale de l'ide qui est hérité de la super classe QMainWindow'''
    def __init__(self):
        super().__init__()
        self.init_ui()

    def init_ui(self):
        #informations de base sur la fenetre
        self.setWindowTitle("Visouale stoudio coude")
        self.setGeometry(100, 100, 1000, 700)
        self.setStyleSheet("background-color: #1E1E1E; color: white;")

        # Zone de texte principale
        self.text_area = QTextEdit(self)
        self.text_area.setStyleSheet("background-color: #1E1E1E; color: #D4D4D4; font-family: Consolas; font-size: 18px;")
        self.text_area.setTabStopDistance(4 * self.text_area.fontMetrics().horizontalAdvance(' '))

        # Highlighter syntaxique
        self.highlighter = SyntaxHighlighter(self.text_area.document())

        # Barre d'outils
        self.toolbar = QToolBar("Main Toolbar", self)
        self.toolbar.setStyleSheet("background-color: #333333; border: none;")
        self.addToolBar(self.toolbar)

        # Boutons de la barre d'outils
        open_icon = QIcon.fromTheme("document-open")
        save_icon = QIcon.fromTheme("document-save")
        run_icon = QIcon.fromTheme("media-playback-start")

        open_action = QAction(open_icon, "Open", self)
        open_action.triggered.connect(self.open_file)
        self.toolbar.addAction(open_action)

        save_action = QAction(save_icon, "Save", self)
        save_action.triggered.connect(self.save_as_file)
        self.toolbar.addAction(save_action)

        run_action = QAction(run_icon, "Run", self)
        run_action.triggered.connect(self.run_code)
        self.toolbar.addAction(run_action)

        # Bouton Run
        self.run_button = QPushButton("Run", self)
        self.run_button.setStyleSheet("background-color: #007ACC; color: white; border: none; padding: 5px 10px; font-size: 14px;")
        self.run_button.clicked.connect(self.run_code)

        # Layout principal
        layout = QVBoxLayout()
        layout.addWidget(self.text_area)
        layout.addWidget(self.run_button)

        container = QWidget()
        container.setLayout(layout)
        self.setCentralWidget(container)

        # Barre de menu
        menubar = self.menuBar()
        menubar.setStyleSheet("background-color: #333333; color: white;")

        file_menu = menubar.addMenu("File")

        open_action_menu = QAction("Open File", self)
        open_action_menu.triggered.connect(self.open_file)
        file_menu.addAction(open_action_menu)

        save_as_action_menu = QAction("Save As", self)
        save_as_action_menu.triggered.connect(self.save_as_file)
        file_menu.addAction(save_as_action_menu)

    ''' METHODES DE L'OBJET SELF : principalement les fonctions pour les triggering des boutons'''

    def open_file(self):
        options = QFileDialog.Options()
        file_path, _ = QFileDialog.getOpenFileName(self, "Open File", "", "Draw++ Files (*.dpp);;All Files (*)", options=options)
        if file_path:
            with open(file_path, "r") as file:
                self.text_area.setText(file.read())

    def save_as_file(self):
        options = QFileDialog.Options()
        file_path, _ = QFileDialog.getSaveFileName(self, "Save File", "", "Draw++ Files (*.dpp);;All Files (*)", options=options)
        if file_path:
            with open(file_path, "w") as file:
                file.write(self.text_area.toPlainText())



    def run_code(self):
        QMessageBox.information(self, "Run", "Exécution du Code Draw++ en cours...")
        
        # Crée le répertoire ".to_run" (Linux/Mac) ou "to_run" (Windows) si nécessaire
        if platform.system() in ["Linux", "Darwin"]:  # Pour Linux ou MacOS
            dir_path = ".to_run"
            file_path = os.path.join(dir_path, "to_execute.dpp")
        elif platform.system() == "Windows":  # Pour Windows
            dir_path = ".to_run"
            #QMessageBox.warning(self, "Attention", "MyDrawpp IDE est encore en test sur Windows")
            file_path = os.path.join(dir_path, "to_execute.dpp")
        else:
            QMessageBox.critical(self, "Erreur", f"MyDrawpp IDE ne fonctionne pas avec {platform.system()}")
            return

        # Création du répertoire si nécessaire
        if not os.path.isdir(dir_path):
            os.makedirs(dir_path)

        # Écriture du contenu du fichier
        with open(file_path, "w") as file:
            file.write(self.text_area.toPlainText())

        # Exécution du script interpreter.py
        try:
            interpreter_script = "interpreter.py"  # Script Python à exécuter
            # Commande pour exécuter le script avec le fichier to_execute.dpp comme argument
            command = ["python", interpreter_script, file_path]

            # Exécute le script interpreter.py avec subprocess
            result = subprocess.run(command, capture_output=True, text=True, check=True)
            
            #il faut clean le result
            

            # Affiche la sortie dans un message box
            QMessageBox.information(self, "Résultat", f"\033[31mErreur syntaxique\033[0m")
        except subprocess.CalledProcessError as e:
            QMessageBox.critical(self, "Erreur", f"Erreur d'exécution :\n{e.stderr}")
        except FileNotFoundError:
            QMessageBox.critical(self, "Erreur", "Le fichier 'interpreter.py' n'a pas été trouvé. Vérifiez son emplacement.")


#partie obligatoire pour run l'appli
if __name__ == "__main__":
    app = QApplication([])
    ide = MyDrawppIDE()
    ide.show()
    app.exec_()