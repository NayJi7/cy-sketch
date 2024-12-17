from PyQt5.QtWidgets import (QApplication, QMainWindow, QTextEdit, QVBoxLayout, QPushButton, QFileDialog, QMessageBox, QWidget, QMenuBar, QAction, QToolBar, QPlainTextEdit)
from PyQt5.QtGui import QTextCharFormat, QColor, QSyntaxHighlighter, QTextCursor, QFont, QIcon,QPainter,QTextFormat
from PyQt5.QtCore import Qt, QProcess,QRect, QSize
import os
import platform
import subprocess


# Ces 3 classes sont utiles pour : avoir les n

class LineNumberArea(QWidget):
    def __init__(self, editor):
        super().__init__(editor)
        self.editor = editor  # Référence à l'éditeur principal

    def sizeHint(self):
        """Retourne la largeur de la marge pour les numéros de ligne."""
        return QSize(self.editor.line_number_area_width(), 0)

    def paintEvent(self, event):
        """Dessine les numéros de ligne."""
        self.editor.line_number_area_paint_event(event)

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

class CodeEditor(QPlainTextEdit):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.lineNumberArea = LineNumberArea(self)

        self.blockCountChanged.connect(self.update_line_number_area_width)
        self.updateRequest.connect(self.update_line_number_area)
        self.cursorPositionChanged.connect(self.highlight_current_line)

        self.update_line_number_area_width(0)
        self.highlight_current_line()

    def line_number_area_width(self):
        """Calcule la largeur de la marge pour les numéros de ligne."""
        digits = len(str(self.blockCount())) + 1
        return 10 + self.fontMetrics().horizontalAdvance('9') * digits

    def update_line_number_area_width(self, _):
        """Met à jour la largeur de la marge pour les numéros de ligne."""
        self.setViewportMargins(self.line_number_area_width(), 0, 0, 0)

    def update_line_number_area(self, rect, dy):
        """Redessine les numéros de ligne après un défilement ou une modification."""
        if dy:
            self.lineNumberArea.scroll(0, dy)
        else:
            self.lineNumberArea.update(0, rect.y(), self.lineNumberArea.width(), rect.height())

        if rect.contains(self.viewport().rect()):
            self.update_line_number_area_width(0)

    def resizeEvent(self, event):
        """Ajuste la zone des numéros de ligne lorsqu'on redimensionne l'éditeur."""
        super().resizeEvent(event)
        cr = self.contentsRect()
        self.lineNumberArea.setGeometry(QRect(cr.left(), cr.top(), self.line_number_area_width(), cr.height()))

    def line_number_area_paint_event(self, event):
        """Dessine les numéros de ligne."""
        painter = QPainter(self.lineNumberArea)
        painter.fillRect(event.rect(), QColor("#2E2E2E"))

        block = self.firstVisibleBlock()
        block_number = block.blockNumber()
        top = self.blockBoundingGeometry(block).translated(self.contentOffset()).top()
        bottom = top + self.blockBoundingRect(block).height()

        while block.isValid() and top <= event.rect().bottom():
            if block.isVisible() and bottom >= event.rect().top():
                number = str(block_number + 1)
                painter.setPen(QColor("gray"))
                painter.drawText(0, int(top), self.lineNumberArea.width(), self.fontMetrics().height(),
                                 Qt.AlignRight, number)

            block = block.next()
            top = bottom
            bottom = top + self.blockBoundingRect(block).height()
            block_number += 1

    def highlight_current_line(self):
        """Met en surbrillance la ligne actuelle."""
        extra_selections = []

        if not self.isReadOnly():
            selection = QTextEdit.ExtraSelection()
            line_color = QColor("#3C3F41")
            selection.format.setBackground(line_color)
            selection.format.setProperty(QTextFormat.FullWidthSelection, True)
            selection.cursor = self.textCursor()
            selection.cursor.clearSelection()
            extra_selections.append(selection)

        self.setExtraSelections(extra_selections)

class MyDrawppIDE(QMainWindow):
    '''Classe principale de l'ide qui est hérité de la super classe QMainWindow'''
    def __init__(self):
        super().__init__()
        self.process = QProcess(self)  
        self.init_ui()


    def init_ui(self):
        #informations de base sur la fenetre
        self.setWindowTitle("Visouale stoudio coude")
        self.setGeometry(100, 100, 1000, 700)
        self.setStyleSheet("background-color: #1E1E1E; color: white;")

        # Zone de texte principale - zone d'édition de code
        self.text_area = QTextEdit(self)
        self.text_area.setStyleSheet("background-color: #1E1E1E; color: #D4D4D4; font-family: Consolas; font-size: 18px;")
        self.text_area.setTabStopDistance(4 * self.text_area.fontMetrics().horizontalAdvance(' '))

        # Zone de texte principale - zone d'édition de code
        self.text_area = CodeEditor(self)
        self.text_area.setStyleSheet("background-color: #1E1E1E; color: #D4D4D4; font-family: Consolas; font-size: 18px;")

        # Highlighter syntaxique
        self.highlighter = SyntaxHighlighter(self.text_area.document())

        # Terminal intégré
        self.terminal = QPlainTextEdit(self)
        self.terminal.setReadOnly(True) # propriété du terminal : ne peut etre que lu et pas écris , pour l'instant
        self.terminal.setStyleSheet("background-color: #494545; color: white; font-family: Consolas; font-size: 14px;")

        # Barre d'outils
        self.toolbar = QToolBar("Main Toolbar", self)
        self.toolbar.setStyleSheet("background-color: #333333; border: none;")
        self.addToolBar(self.toolbar)

       # Boutons de la barre d'outils
        open_icon = QIcon.fromTheme("document-open")
        save_icon = QIcon.fromTheme("document-save")

        open_action = QAction(open_icon, "Open", self)
        open_action.setFont(QFont("Consolas", 12))  # Changer la taille du texte ici
        open_action.triggered.connect(self.open_file)
        self.toolbar.addAction(open_action)

        save_action = QAction(save_icon, "Save", self)
        save_action.setFont(QFont("Consolas", 12))  # Changer la taille du texte ici
        save_action.triggered.connect(self.save_as_file)
        self.toolbar.addAction(save_action)

        # Bouton Run 
        self.run_button = QPushButton("Run", self)
        self.run_button.setIcon(QIcon("green-play-button-png.ico"))  # Ajout de l'icône
        self.run_button.setStyleSheet("background-color: #007ACC; color: white; border: none; padding: 5px 10px; font-size: 14px;")
        self.run_button.clicked.connect(self.run_code)

        # Layout principal
        layout = QVBoxLayout()
        layout.addWidget(self.text_area)
        layout.addWidget(self.run_button)
        layout.addWidget(self.terminal)

        container = QWidget()
        container.setLayout(layout)
        self.setCentralWidget(container)

        #explications : 
        #QProcess c'est une classe de PyQt5 qui permet d'executer des programmes externes et des commandes système dans un processus séparé
        # et quand un programme/commande est fait avec QProcess, alors on peut en sortir un stdout et un stderr
        # pour les capturer on doit utiliser 2 signaux qui sont natifs a QProcess qui sont : 
                # readyReadStandardOutput -> quand il y a un truc qui ressort de la commande effectuée
                # readyReadStandardError -> quand il y a une erreur (non voulu donc pas les nôtres) qui ressort de la commande effectuée


        # Connecte le QProcess aux méthodes pour afficher les sorties
        self.process.readyReadStandardOutput.connect(self.display_output) # on connecte la sortie a ma méthode display_output qui se charge de récup et afficher
        #self.process.readyReadStandardError.connect(self.display_error) # same avec ma méthode display_error: pour l'instant pas besoin

        # Barre de menu
        menubar = self.menuBar()
        menubar.setStyleSheet("background-color: #333333; color: white;")

        file_menu = menubar.addMenu("File")

        run_icon = QIcon("green-play-button-png.ico")  # Charger ton icône personnalisée
        run_action = QAction(run_icon, "Run", self)
        run_action.triggered.connect(self.run_code)
        self.toolbar.addAction(run_action)

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
        # Préparation du fichier temporaire
        dir_path = ".to_run"
        if not os.path.exists(dir_path):
            os.makedirs(dir_path)

        file_path = os.path.join(dir_path, "to_execute.dpp")
        with open(file_path, "w") as file:
            file.write(self.text_area.toPlainText())

        interpreter_script = "interpreter.py"

        # Commande à exécuter
        command = ["python3", interpreter_script, file_path]
        self.terminal.clear()  # Nettoie le terminal avant d'exécuter
        #self.terminal.appendPlainText(f"Running command: {' '.join(command)}\n")

        # Démarre le processus
        self.process.start("python3", [interpreter_script, file_path])


    # gestion de l'affiche dans le terminal


    def display_output(self):
        output = self.process.readAllStandardOutput().data().decode() #.data et .decode -> converti en chaine de caracteres visible sinon on comprend rien

        for line in output.splitlines():  # Découpe en lignes
            if "31m" in line:  # Si "31m" est détecté -> Rouge
                line = line.replace("[31m","")
                line = line.replace("[0m","")
                self.append_colored_text(line + "\n", QColor("red")) #
            elif "34m" in line:  # Si "34m" est détecté -> Bleu
                line = line.replace("[34m","")
                line = line.replace("[0m","")
                self.append_colored_text(line + "\n", QColor("blue"))
            else:
                self.terminal.appendPlainText(line)  # Affiche normalement
    #def display_error(self):
        error_output = self.process.readAllStandardError().data().decode()
        for line in error_output.splitlines():  # Découpe en lignes
            if "31m" in line:  # Si "31m" est détecté -> Rouge
                self.append_colored_text(line + "\n", QColor("red"))
            elif "34m" in line:  # Si "34m" est détecté -> Bleu
                self.append_colored_text(line + "\n", QColor("blue"))
            else:
                self.terminal.appendPlainText(line)  # Affiche normalement

    def append_colored_text(self, text, color=QColor("color")):
        """Ajoute une ligne de texte colorée dans le terminal."""
        cursor = self.terminal.textCursor()
        cursor.movePosition(QTextCursor.End)  # Place le curseur à la fin
        format = QTextCharFormat()
        format.setForeground(color)  # Définit la couleur (rouge par défaut)
        cursor.insertText(text, format)  # Insère le texte coloré
        self.terminal.setTextCursor(cursor)
        self.terminal.ensureCursorVisible()  # S'assure que le texte reste visible


#partie obligatoire pour run l'appli
if __name__ == "__main__":
    app = QApplication([])
    ide = MyDrawppIDE()
    ide.show()
    app.exec_()