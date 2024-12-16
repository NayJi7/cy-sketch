from PyQt5.QtWidgets import (QApplication, QMainWindow, QTextEdit, QVBoxLayout, QPushButton, QFileDialog, QMessageBox, QWidget, QMenuBar, QAction, QToolBar, QPlainTextEdit)
from PyQt5.QtGui import QTextCharFormat, QColor, QSyntaxHighlighter, QTextCursor, QFont, QIcon
from PyQt5.QtCore import Qt, QProcess
import os
import platform

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
        keywords = ["if", "else", "for", "while", "return", "def", "class"]
        self.add_rules(keywords, keyword_format)
        #coloration des fonctions dpp
        keyword_format2 = QTextCharFormat()
        keyword_format2.setForeground(QColor("#ff5733"))
        keyword_format2.setFontWeight(QFont.Bold)
        functions_word1 = ["rotate", "cursor","draw_circle"]
        self.add_rules(functions_word1,keyword_format2)
        #coloration des modules d'importation
        keyword_format3 = QTextCharFormat()
        keyword_format3.setForeground(QColor("#e873f3"))
        keyword_format3.setFontWeight(QFont.Bold)
        functions_word2 = ["import", "from"]
        self.add_rules(functions_word2,keyword_format3)

        

    def add_rules(self, patterns, text_format):
        '''méthode pour ajouter une règle de coloration'''
        for pattern in patterns:
            regex = f"\\b{pattern}\\b"
            self.rules.append((regex, text_format))

    def highlightBlock(self, text):
        for regex, text_format in self.rules:
            start = 0
            while (match := self.findMatch(regex, text, start)):
                start, length = match
                self.setFormat(start, length, text_format)
                start += length

    def findMatch(self, regex, text, start):
        import re
        match = re.search(regex, text[start:])
        if match:
            return start + match.start(), match.end() - match.start()
        return None

class MyDrawppIDE(QMainWindow):
    '''Classe principale de l'ide qui est hérité de la super classe QMainWindow'''
    def __init__(self):
        super().__init__()
        self.init_ui()

    def init_ui(self):
        self.setWindowTitle("MyDrawPP IDE")
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

    def read_output(self):
        output = self.process.readAllStandardOutput().data().decode()
        QMessageBox.information(self, "Résultat", output)

    def read_error(self):
        error = self.process.readAllStandardError().data().decode()
        QMessageBox.critical(self, "Erreur d'exécution", error)

    def on_process_finished(self):
        QMessageBox.information(self, "Terminé", "L'exécution du code est terminée.")

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
        # Sauvegarde le contenu du fichier temporaire
        if platform.system() == "Linux" or platform.system() == "Darwin":
            if not os.path.isdir(".to_run"):
                os.makedirs(".to_run")
            file_path = "./.to_run/to_execute.dpp"
            with open(file_path, "w") as file:
                file.write(self.text_area.toPlainText())
            
            # Exécute le script interpreter.py avec le fichier temporaire en argument
            self.process = QProcess(self)
            self.process.setProgram("python3")  # Utilise "python3" sous Linux/macOS
            self.process.setArguments(["interpreter.py", file_path])
            self.process.start()
            
            self.process.finished.connect(self.on_process_finished)
            self.process.readyReadStandardOutput.connect(self.read_output)
            self.process.readyReadStandardError.connect(self.read_error)

        elif platform.system() == "Windows":
            QMessageBox.warning(self, "Attention", "MyDrawpp IDE n'est pas encore compatible avec Windows")
        else:
            QMessageBox.critical(self, "Erreur", "MyDrawpp IDE ne fonctionne pas avec " + platform.system())

if __name__ == "__main__":
    app = QApplication([])
    ide = MyDrawppIDE()
    ide.show()
    app.exec_()