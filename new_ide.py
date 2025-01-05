from PyQt5.QtWidgets import (QApplication, QMainWindow, QTextEdit, QVBoxLayout, QTabWidget,QPushButton, QFileDialog, QMessageBox, QWidget, QMenuBar, QAction, QToolBar, QPlainTextEdit)
from PyQt5.QtGui import QTextCharFormat, QColor, QSyntaxHighlighter, QTextCursor, QFont, QIcon,QPainter,QTextFormat
from PyQt5.QtCore import Qt, QProcess,QRect, QSize,QRegExp
import os
import platform
import subprocess
import re
import sys
if platform.system() == "Windows":
    from PyQt5.QtWinExtras import QtWin   #Pour Windows uniquement  #type: ignore 

global ongletCount
ongletCount = 0


# Ces 2 classes sont là pour mettre les numéros de ligne, pas important et juste 
# esthétique, rien à modifier dedans
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



# classe pour surligner les mots terminaux dans le langage (return, from , if, cursor...)
#  TBD <à mettre à jour avec tous les mots car modifié car ajout d'onglets à fait tout crash>
class SyntaxHighlighter(QSyntaxHighlighter):
    def __init__(self, document):
        super().__init__(document)
        self.rules = []

        # Définir les formats
        self.keyword_format = QTextCharFormat()
        self.keyword_format.setForeground(QColor("#ff5733"))  # Violet
        self.keyword_format.setFontWeight(QFont.Bold)

        # Ajouter des règles
        self.add_rules(["import", "from", "def", "class", "self"], self.keyword_format)

    def add_rules(self, patterns, text_format):
        """Ajoute des règles de surlignage."""
        for pattern in patterns:
            # Utilisation des expressions régulières avec \b pour les mots entiers
            regex = QRegExp(rf"\b{pattern}\b")
            self.rules.append((regex, text_format))

    def highlightBlock(self, text):
        """Applique les règles de surlignage au bloc de texte donné."""

        print("HighlightBlock called")


        for regex, text_format in self.rules:
            index = regex.indexIn(text)
            while index >= 0:  # Tant qu'il y a des correspondances
                length = regex.matchedLength()
                self.setFormat(index, length, text_format)
                index = regex.indexIn(text, index + length)


class MyDrawppIDE(QMainWindow):
    '''Classe principale de l'IDE'''
    def __init__(self):
        super().__init__()
        self.process = QProcess(self)
        self.setWindowIcon(QIcon("Dpp_circle.ico"))
        self.init_ui()

    def init_ui(self):
        # Configuration de la fenêtre principale
        global ongletCount
        self.setWindowTitle(f'Onglet actif : Onglet {ongletCount}')
        self.setGeometry(100, 100, 1000, 700)
        self.setStyleSheet("background-color: #1E1E1E; color: white;")
        

        # Widget principal avec onglets
        self.tab_widget = QTabWidget(self)
        self.setCentralWidget(self.tab_widget)

        # Boutons pour la barre d'outils
        self.toolbar = QToolBar("Main Toolbar", self)
        self.toolbar.setStyleSheet("background-color: #333333; border: none;")
        self.addToolBar(self.toolbar)

        open_icon = QIcon.fromTheme("document-open")
        save_icon = QIcon.fromTheme("document-save")
        new_tab_icon = QIcon.fromTheme("new-tab")
        new_window_icon = QIcon.fromTheme("new-window")

        open_action = QAction(open_icon, "Open", self)
        open_action.triggered.connect(self.open_file)
        self.toolbar.addAction(open_action)

        save_action = QAction(save_icon, "Save", self)
        save_action.triggered.connect(self.save_as_file)
        self.toolbar.addAction(save_action)

        new_tab_action = QAction(new_tab_icon, "New Tab", self)
        new_tab_action.triggered.connect(self.open_new_tab)
        self.toolbar.addAction(new_tab_action)

        new_window_action = QAction(new_window_icon, "New Window", self)
        new_window_action.triggered.connect(self.show_new_window)
        self.toolbar.addAction(new_window_action)


        run_icon = QIcon("green-play-button-png.ico")
        run_action = QAction(run_icon, "Run", self)
        run_action.triggered.connect(self.run_code)
        self.toolbar.addAction(run_action)

        # Connecter le changement d'onglet pour mettre à jour le titre
        self.tab_widget.currentChanged.connect(self.update_window_title)
        # Ajoute un premier onglet par défaut
        self.open_new_tab()

    def open_new_tab(self):
        """Créer un nouvel onglet avec un éditeur et un terminal."""
        print("Création d'un nouvel onglet")

        global ongletCount
        ongletCount+=1

        # Crée un nouvel onglet et son layout
        new_tab = QWidget()
        layout = QVBoxLayout()

        # Création de l'éditeur
        editor = CodeEditor(self)
        editor.setStyleSheet("background-color: #1E1E1E; color: #D4D4D4; font-family: Consolas; font-size: 18px;")
        editor.setTabStopDistance(4 * editor.fontMetrics().horizontalAdvance(' '))

        # Ajouter le surlignage syntaxique
        highlighter = SyntaxHighlighter(editor.document())

        # Création du terminal
        terminal = QPlainTextEdit(self)
        terminal.setReadOnly(True)
        terminal.setStyleSheet("background-color: #000000; color: white; font-family: Consolas; font-size: 14px;")

        # Ajouter les widgets au layout
        layout.addWidget(editor)
        layout.addWidget(terminal)

        new_tab.setLayout(layout)

        # Stocker les références dans un dictionnaire, car je dois faire un self pour que le highlight marche

        #   Chaque fois qu'un nouvel onglet est créé :

        # Un nouvel éditeur est créé.
        # Un nouveau surligneur (SyntaxHighlighter) est attaché à cet éditeur.
        # Les références sont stockées dans le dictionnaire pour garantir que chaque éditeur et chaque surligneur restent en vie.
        
        if not hasattr(self, 'tabs_data'):
            self.tabs_data = {}
        #comme ca, chacun a son terminal, son editeur, et son hightlighter
        self.tabs_data[new_tab] = {'editor': editor, 'terminal': terminal, 'highlighter': highlighter}

        # Ajouter l'onglet 
        tabName = f'Onglet {ongletCount}'
        self.tab_widget.addTab(new_tab, tabName)
        self.tab_widget.setCurrentWidget(new_tab)

    def update_window_title(self, index):
        """Met à jour le titre de la fenêtre selon l'onglet actif."""
        if index == -1:
            self.setWindowTitle("Visouale Stoudio Coude")  # Aucun onglet actif
        else:
            current_tab_title = self.tab_widget.tabText(index)
            self.setWindowTitle(f"Visouale Stoudio Coude - {current_tab_title}")


    def open_file(self):
        """Ouvre un fichier dans l'onglet actif."""
        options = QFileDialog.Options()
        file_path, _ = QFileDialog.getOpenFileName(self, "Open File", "", "Draw++ Files (*.dpp);;All Files (*)", options=options)
        if file_path:
            with open(file_path, "r") as file:
                current_tab = self.tab_widget.currentWidget()
                editor = current_tab.layout().itemAt(0).widget()
                editor.setPlainText(file.read())
                self.tab_widget.setTabText(self.tab_widget.currentIndex(), os.path.basename(file_path))

    def save_as_file(self):
        """Sauvegarde le contenu de l'éditeur actif."""
        options = QFileDialog.Options()
        file_path, _ = QFileDialog.getSaveFileName(self, "Save File", "", "Draw++ Files (*.dpp);;All Files (*)", options=options)
        if file_path:
            current_tab = self.tab_widget.currentWidget()
            editor = current_tab.layout().itemAt(0).widget()
            with open(file_path, "w") as file:
                file.write(editor.toPlainText())
            self.tab_widget.setTabText(self.tab_widget.currentIndex(), os.path.basename(file_path))

    def run_code(self):
        """Exécute le code de l'éditeur actif et affiche la sortie dans le terminal actif."""
        current_tab = self.tab_widget.currentWidget()
        if not current_tab:
            return  # Aucun onglet actif

        # Récupérer l'éditeur et le terminal associés à l'onglet actif
        tab_data = self.tabs_data.get(current_tab)
        if not tab_data:
            return

        editor = tab_data['editor']
        terminal = tab_data['terminal']

        # Préparation du fichier temporaire
        dir_path = ".to_run"
        if not os.path.exists(dir_path):
            os.makedirs(dir_path)

        file_path = os.path.join(dir_path, "to_execute.dpp")
        with open(file_path, "w") as file:
            file.write(editor.toPlainText())

        interpreter_script = "interpreter.py"

        # Nettoyer le terminal avant d'exécuter
        terminal.clear()

        # Créer un nouveau processus pour cet onglet
        process = QProcess(self)
        tab_data['process'] = process  # Stocker le processus dans le dictionnaire

        # Connecter les sorties au terminal actif
        process.readyReadStandardOutput.connect(lambda: self.display_output(process, terminal))
        process.readyReadStandardError.connect(lambda: self.display_error(process, terminal))

        # Configurer la commande pour l'exécution
        if platform.system() == "Windows":
            command = ["python", interpreter_script, file_path]
        else:
            command = ["python3", interpreter_script, file_path]

        # Démarrer le processus
        process.start(command[0], command[1:])



    def display_output(self, process, terminal):
        """Affiche les sorties dans le terminal spécifié avec des couleurs simples pour certaines séquences."""
        output = process.readAllStandardOutput().data().decode('utf-8', errors='ignore')

        for line in output.splitlines():  # Découpe en lignes
            if "31m" in line:  # Si "31m" est détecté -> Rouge
                line = line.replace("[31m", "").replace("[0m", "")
                self.append_colored_text(terminal, line + "\n", QColor("#cf4016"))
            elif "34m" in line:  # Si "34m" est détecté -> Bleu
                line = line.replace("[34m", "").replace("[0m", "")
                self.append_colored_text(terminal, line + "\n", QColor("#555df5"))
            else:  # Texte sans séquence ANSI, affiché normalement
                terminal.appendPlainText(line)

    def append_colored_text(self, terminal, text, color=QColor("white")):
        """Ajoute une ligne de texte colorée dans le terminal spécifié."""
        cursor = terminal.textCursor()
        cursor.movePosition(QTextCursor.End)  # Place le curseur à la fin
        text_format = QTextCharFormat()
        text_format.setForeground(color)  # Définit la couleur
        cursor.insertText(text, text_format)  # Insère le texte coloré
        terminal.setTextCursor(cursor)
        terminal.ensureCursorVisible()  # S'assure que le texte reste visible

    def show_new_window(self):
        self.w = AnotherWindow()
        self.w.show()


class AnotherWindow(MyDrawppIDE):
    def __init__(self):
        super().__init__()





if __name__ == "__main__":
    app = QApplication(sys.argv)

    if platform.system() == "Windows":
        icon = QIcon("Dpp_circle.ico")
        app.setWindowIcon(icon)
        if hasattr(QtWin, 'setCurrentProcessExplicitAppUserModelID'):
            myappid = 'mon.application.ide.version1.0'
            QtWin.setCurrentProcessExplicitAppUserModelID(myappid)

    window = MyDrawppIDE()
    window.show()
    sys.exit(app.exec_())