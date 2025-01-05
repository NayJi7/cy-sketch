from PyQt5.QtWidgets import (QApplication, QMainWindow, QTextEdit, QVBoxLayout, QTabWidget,QPushButton, QFileDialog, QMessageBox, QWidget, QMenuBar, QAction, QToolBar, QPlainTextEdit, QSizePolicy)
from PyQt5.QtGui import QTextCharFormat, QColor, QSyntaxHighlighter, QTextCursor, QFont, QIcon,QPainter,QTextFormat
from PyQt5.QtCore import Qt, QProcess,QRect, QSize,QRegExp
import os
import platform
import subprocess
import re
import sys
from time import sleep
if platform.system() == "Windows":
    from PyQt5.QtWinExtras import QtWin   #Pour Windows uniquement  #type: ignore 

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
        self.keyword_format.setForeground(QColor("#ff5733"))  # Orange
        self.keyword_format.setFontWeight(QFont.Bold)

        # Nouveau format pour les fonctions de dessin
        self.drawing_format = QTextCharFormat()
        self.drawing_format.setForeground(QColor("#00ff00"))  # Vert
        self.drawing_format.setFontWeight(QFont.Bold)

        # Nouveau format pour les mots-clés de contrôle
        self.control_format = QTextCharFormat()
        self.control_format.setForeground(QColor("#42a5f5"))  # Bleu clair
        self.control_format.setFontWeight(QFont.Bold)

        # Ajouter des règles
        self.add_rules(["import", "from", "def", "class", "self"], self.keyword_format)
        self.add_rules(["draw circle", "draw line", "draw square", "draw rectangle"], self.drawing_format)
        self.add_rules(["var", "for", "while"], self.control_format)

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
        
        # Store Draw++ location once at initialization
        current_dir = os.path.dirname(os.path.abspath(__file__))
        
        if os.path.basename(current_dir) == "Draw++":
            # Case 1: Running from ide.py in Draw++ folder
            self.draw_folder = current_dir
        elif "_MEI" in current_dir:
            # Case 2: Running from .exe (PyInstaller temp directory)
            self.draw_folder = "Draw++"
        else:
            # Case 3: Fallback
            raise Exception("Could not locate Draw++ folder. You are here: "+current_dir)

        
        self.setWindowIcon(QIcon(os.path.join(self.draw_folder, "IDE", "Dpp_circle.ico")))
        self.history_file = os.path.join(self.draw_folder, "IDE", ".history")
        self.init_ui()
        self.load_last_file()

    def init_ui(self):
        # Configuration de la fenêtre principale
        self.setGeometry(100, 100, 1000, 700)
        self.setStyleSheet("background-color: #1E1E1E; color: white;")
        
        # Get the absolute path to the images directory
        ide_dir = os.path.join(self.draw_folder, "IDE")
        cross_icon_path = os.path.join(ide_dir, "white-cross.png")
        # Convert to URL format with forward slashes for CSS
        cross_icon_url = cross_icon_path.replace(os.sep, '/')
        
        # Widget principal avec onglets
        self.tab_widget = QTabWidget(self)
        self.setCentralWidget(self.tab_widget)
        self.tab_widget.setTabsClosable(True)  # Enable tab close buttons
        self.tab_widget.tabCloseRequested.connect(self.close_tab)  # Connect close signal

        self.tab_widget.setStyleSheet(f"""
            QTabWidget::pane {{
                border: 1px solid #444444;
                background: #1E1E1E;
                top: -1px;
            }}
            QTabBar::tab {{
                background: #333333;
                color: white;
                padding: 8px 15px;
                padding-right: 0px;
                padding-bottom: 12px;
                border: 1px solid #444444;
                margin-right: 2px;
                margin-bottom: -1px;
                min-width: 80px;
                max-width: 180px;
                font-size: 12px;
                border-top-left-radius: 6px;
                border-top-right-radius: 6px;
            }}
            QTabBar::tab:selected {{
                background: #1E1E1E;
                border-bottom: none;
                padding-bottom: 13px;
                font-size: 13px;
                font-weight: bold;
            }}
            QTabBar::tab:hover {{
                background: #444444;
            }}
            QTabBar::close-button {{
                image: url({cross_icon_url});
                background: transparent;
                border: none;
                padding: 2px;
                width: 25px;
                height: 25px;
                subcontrol-position: right;
                subcontrol-origin: padding;
                position: absolute;
                right: 8px;
            }}
            QTabBar::close-button:hover {{
                background: #666666;
                border-radius: 2px;
            }}
        """)

        # Boutons pour la barre d'outils
        self.toolbar = QToolBar("Main Toolbar", self)
        self.toolbar.setStyleSheet("""
            QToolBar {
                background-color: #1E1E1E;
                border: none;
                padding: 0px;
            }
            QToolBar QToolButton {
                background-color: #333333;
                border: none;
                padding: 5px;
                color: white;
            }
            QToolBar QToolButton:hover {
                background-color: #555555;
            }
            QToolBar QToolButton:pressed {
                background-color: #666666;
            }
            QToolBar QToolButton#runButton {
                background-color: #2E7D32;
            }
            QToolBar QToolButton#runButton:hover {
                background-color: #388E3C;
            }
            QToolBar QToolButton#runButton:pressed {
                background-color: #1B5E20;
            }
        """)
        self.addToolBar(self.toolbar)

        # Boutons de gauche (fichiers)
        open_icon = QIcon.fromTheme("document-open")
        save_icon = QIcon.fromTheme("document-save")
        save_as_icon = QIcon.fromTheme("document-save-as")
        new_tab_icon = QIcon.fromTheme("new-tab")
        new_window_icon = QIcon.fromTheme("new-window")

        open_action = QAction(open_icon, "Open", self)
        open_action.triggered.connect(self.open_file)
        self.toolbar.addAction(open_action)

        # Séparateur
        separator1 = QWidget()
        separator1.setFixedWidth(1)
        separator1.setStyleSheet("background-color: #1E1E1E;")
        self.toolbar.addWidget(separator1)

        save_action = QAction(save_icon, "Save", self)
        save_action.triggered.connect(self.save_file)
        self.toolbar.addAction(save_action)

        # Séparateur
        separator2 = QWidget()
        separator2.setFixedWidth(1)
        separator2.setStyleSheet("background-color: #1E1E1E;")
        self.toolbar.addWidget(separator2)

        save_as_action = QAction(save_as_icon, "Save As", self)
        save_as_action.triggered.connect(self.save_as_file)
        self.toolbar.addAction(save_as_action)

        # Séparateur
        separator3 = QWidget()
        separator3.setFixedWidth(1)
        separator3.setStyleSheet("background-color: #1E1E1E;")
        self.toolbar.addWidget(separator3)

        new_tab_action = QAction(new_tab_icon, "New Tab", self)
        new_tab_action.triggered.connect(self.open_new_tab)
        self.toolbar.addAction(new_tab_action)

        # Séparateur
        separator4 = QWidget()
        separator4.setFixedWidth(1)
        separator4.setStyleSheet("background-color: #1E1E1E;")
        self.toolbar.addWidget(separator4)

        new_window_action = QAction(new_window_icon, "New Window", self)
        new_window_action.triggered.connect(self.show_new_window)
        self.toolbar.addAction(new_window_action)

        # Ajouter un widget extensible pour pousser les boutons suivants à droite
        spacer = QWidget()
        spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        self.toolbar.addWidget(spacer)

        # Boutons de droite (exécution)
        run_icon = QIcon.fromTheme("document-open")
        run_action = QAction(run_icon, "Run", self)
        run_action.triggered.connect(self.run_code)
        self.toolbar.addAction(run_action)
        # Définir l'ID pour le style du bouton Run
        run_button = self.toolbar.widgetForAction(run_action)
        if run_button:
            run_button.setObjectName("runButton")

        # Séparateur
        separator5 = QWidget()
        separator5.setFixedWidth(1)
        separator5.setStyleSheet("background-color: #1E1E1E;")
        self.toolbar.addWidget(separator5)

        compile_icon = QIcon.fromTheme("document-open")
        compile_action = QAction(compile_icon, "Compile", self)
        compile_action.triggered.connect(self.compile_code)
        self.toolbar.addAction(compile_action)

        # Séparateur
        separator6 = QWidget()
        separator6.setFixedWidth(1)
        separator6.setStyleSheet("background-color: #1E1E1E;")
        self.toolbar.addWidget(separator6)

        debug_icon = QIcon.fromTheme("document-open")
        debug_action = QAction(debug_icon, "Debug", self)
        debug_action.triggered.connect(self.debug_code)
        self.toolbar.addAction(debug_action)

        # Connecter le changement d'onglet pour mettre à jour le titre
        self.tab_widget.currentChanged.connect(self.update_window_title)
        # Ajoute un premier onglet par défaut
        self.open_new_tab()

    def open_new_tab(self):
        """Créer un nouvel onglet avec un éditeur et un terminal."""
        print("Création d'un nouvel onglet")

        # Crée un nouvel onglet et son layout
        new_tab = QWidget()
        layout = QVBoxLayout()

        # Création de l'éditeur
        editor = CodeEditor(self)
        editor.setStyleSheet("background-color: #1E1E1E; color: #D4D4D4; font-family: Consolas; font-size: 18px;")
        editor.setTabStopDistance(4 * editor.fontMetrics().horizontalAdvance(' '))
        editor.document().setModified(False)  # Initialiser l'état de modification

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

        if not hasattr(self, 'tabs_data'):
            self.tabs_data = {}
        self.tabs_data[new_tab] = {'editor': editor, 'terminal': terminal, 'highlighter': highlighter}

        # Ajouter l'onglet 
        tabName = f'New Tab'
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
        """Ouvre un fichier dans un nouvel onglet ou bascule vers l'onglet existant."""
        options = QFileDialog.Options()
        file_path, _ = QFileDialog.getOpenFileName(self, "Open File", "", "Draw++ Files (*.dpp);;All Files (*)", options=options)
        if file_path:
            # Vérifier si le fichier est déjà ouvert
            file_name = os.path.basename(file_path)
            for i in range(self.tab_widget.count()):
                if self.tab_widget.tabText(i) == file_name:
                    # Le fichier est déjà ouvert, basculer vers cet onglet
                    self.tab_widget.setCurrentIndex(i)
                    return

            # Check if current tab is a new tab
            current_index = self.tab_widget.currentIndex()
            if current_index != -1 and self.tab_widget.tabText(current_index).startswith('New Tab'):
                # Use current tab instead of creating a new one
                current_tab = self.tab_widget.currentWidget()
                editor = current_tab.layout().itemAt(0).widget()
            else:
                # Create new tab
                self.open_new_tab()
                current_tab = self.tab_widget.currentWidget()
                editor = current_tab.layout().itemAt(0).widget()
            
            with open(file_path, "r") as file:
                editor.setPlainText(file.read())
                self.tab_widget.setTabText(self.tab_widget.currentIndex(), file_name)
                editor.document().setModified(False)
                self.save_file_history(os.path.abspath(file_path))
            self.update_window_title(self.tab_widget.currentIndex())

    def save_file(self):
        """Sauvegarde le fichier actif. Si c'est un nouveau fichier, demande où sauvegarder."""
        current_tab = self.tab_widget.currentWidget()
        if not current_tab:
            return

        current_file_name = self.tab_widget.tabText(self.tab_widget.currentIndex())
        if current_file_name.startswith('New Tab'):  # C'est un nouveau fichier
            self.save_as_file()  # Demander où sauvegarder
        else:
            # Sauvegarder dans le fichier existant
            editor = current_tab.layout().itemAt(0).widget()
            try:
                with open(current_file_name, "w") as file:
                    file.write(editor.toPlainText())
                editor.document().setModified(False)
                self.save_file_history(os.path.abspath(current_file_name))
            except Exception as e:
                QMessageBox.warning(self, "Save Error", f"Could not save file: {str(e)}")

    def save_as_file(self):
        """Sauvegarde le contenu de l'éditeur actif sous un nouveau nom."""
        current_tab_name = self.tab_widget.tabText(self.tab_widget.currentIndex())
        if current_tab_name.startswith('New Tab'):
            current_tab_name = "MySuperDraw"
            
        options = QFileDialog.Options()
        file_path, _ = QFileDialog.getSaveFileName(
            self, 
            "Save File", 
            current_tab_name,
            "Draw++ Files (*.dpp);;All Files (*)", 
            options=options
        )
        
        if file_path:
            current_tab = self.tab_widget.currentWidget()
            editor = current_tab.layout().itemAt(0).widget()
            try:
                with open(file_path, "w") as file:
                    file.write(editor.toPlainText())
                self.tab_widget.setTabText(self.tab_widget.currentIndex(), os.path.basename(file_path))
                editor.document().setModified(False)
                self.save_file_history(os.path.abspath(file_path))
            except Exception as e:
                QMessageBox.warning(self, "Save Error", f"Could not save file: {str(e)}")

    def execute_code(self, mode="run"):
        """Exécute le code de l'éditeur actif selon le mode spécifié (run/compile/debug)."""
        current_tab = self.tab_widget.currentWidget()
        if not current_tab:
            return  # Aucun onglet actif

        # Récupérer l'éditeur et le terminal associés à l'onglet actif
        tab_data = self.tabs_data.get(current_tab)
        if not tab_data:
            return

        editor = tab_data['editor']
        terminal = tab_data['terminal']

        # Vérifier si le fichier doit être sauvegardé
        current_file_name = self.tab_widget.tabText(self.tab_widget.currentIndex())
        if current_file_name.startswith('New Tab'):
            reply = QMessageBox.question(
                self,
                "Save Required",
                "You need to save your file before running it. Would you like to save now?",
                QMessageBox.Save | QMessageBox.Cancel,
                QMessageBox.Save
            )
            
            if reply == QMessageBox.Save:
                self.save_as_file()
                # Vérifier si l'utilisateur a annulé la sauvegarde
                if self.tab_widget.tabText(self.tab_widget.currentIndex()).startswith('New Tab'):
                    return  # L'utilisateur a annulé la sauvegarde, on arrête l'exécution
            else:
                return  # L'utilisateur a cliqué sur Annuler, on arrête l'exécution

        # Configurer la commande pour l'exécution
        if platform.system() == "Windows":
            make_command = "mingw32-make"
        else:
            make_command = "make"

        # Trouver le dossier Draw++
        draw_folder = self.draw_folder

        # Créer le fichier .to_compile.dpp dans le dossier Draw++
        compile_file_path = os.path.join(draw_folder, ".to_compile.dpp")
        try:
            with open(compile_file_path, "w") as file:
                file.write(editor.toPlainText())
        except Exception as e:
            QMessageBox.critical(self, "Error", f"Could not write to {compile_file_path}: {str(e)}")
            return

        filename = "NAME=" + self.tab_widget.tabText(self.tab_widget.currentIndex())
        filename = filename.replace('.dpp', '')

        # Nettoyer le terminal avant d'exécuter
        terminal.clear()

        # Créer un nouveau processus pour cet onglet
        process = QProcess(self)
        tab_data['process'] = process  # Stocker le processus dans le dictionnaire

        # Définir le répertoire de travail sur le dossier Draw++
        process.setWorkingDirectory(draw_folder)

        # Connecter les sorties au terminal actif
        process.readyReadStandardOutput.connect(lambda: self.display_output(process, terminal))
        # process.readyReadStandardError.connect(lambda: self.display_error(process, terminal))

        # Configurer la commande et les arguments selon le mode
        command = make_command
        if mode == "compile":
            arguments = ["compile", filename]
        elif mode == "debug":
            arguments = ["DEBUG=1", filename]
        else:  # mode run
            arguments = [filename]

        # Démarrer le processus avec la commande et les arguments
        process.start(command, arguments)

    def run_code(self):
        """Exécute le code en mode normal."""
        self.execute_code("run")

    def compile_code(self):
        """Compile le code sans l'exécuter."""
        self.execute_code("compile")

    def debug_code(self):
        """Exécute le code en mode debug."""
        self.execute_code("debug")

    def display_output(self, process, terminal):
        """Affiche les sorties dans le terminal spécifié avec des couleurs simples pour certaines séquences."""
        output = process.readAllStandardOutput().data().decode('utf-8', errors='ignore')
        
        for line in output.splitlines():
            # Handle debug lines without coloring them
            if line.startswith('-#red '):
                line = line.replace('-#red ', '')
                self.append_colored_text(terminal, line + "\n", QColor("red"))
                if "ExecutionError" in line:  # Si c'est une erreur d'exécution, on arrête le processus
                    process.kill()
            elif line.startswith('-#green '):
                line = line.replace('-#green ', '')
                self.append_colored_text(terminal, line + "\n", QColor("green"))
            elif line.startswith('-#blue '):
                line = line.replace('-#blue ', '')
                self.append_colored_text(terminal, line + "\n", QColor("blue"))
            else:
                # Les lignes sans tag sont affichées en blanc
                self.append_colored_text(terminal, line + "\n", QColor("white"))

    def append_colored_text(self, terminal, text, color=QColor("white")):
        """Ajoute une ligne de texte colorée dans le terminal spécifié."""
        cursor = terminal.textCursor()
        cursor.movePosition(QTextCursor.End)  # Place le curseur à la fin
        text_format = QTextCharFormat()
        text_format.setForeground(color)  # Définit la couleur
        cursor.insertText(text, text_format)  # Insère le texte coloré
        terminal.setTextCursor(cursor)
        terminal.ensureCursorVisible()  # S'assure que le texte reste visible
        text_format.setForeground(QColor("white"))  # Définit la couleur


    def display_error(self, process, terminal):
        """Affiche les erreurs dans le terminal spécifié."""
        error_output = process.readAllStandardError().data().decode('utf-8', errors='ignore')

        for line in error_output.splitlines():  # Découpe en lignes
            self.append_colored_text(terminal, line + "\n", QColor("yellow"))  # Rouge par défaut pour les erreurs

    def show_new_window(self):
        self.w = AnotherWindow()
        self.w.show()

    def close_tab(self, index):
        """Ferme l'onglet à l'index spécifié avec une demande de sauvegarde si nécessaire."""
        tab = self.tab_widget.widget(index)
        if not tab:
            return

        editor = tab.layout().itemAt(0).widget()
        if editor.document().isModified():  # Si le document a été modifié
            reply = QMessageBox.question(
                self,
                "Save Changes",
                "Do you want to save changes before closing?",
                QMessageBox.Save | QMessageBox.Discard | QMessageBox.Cancel
            )

            if reply == QMessageBox.Save:
                # Si l'utilisateur veut sauvegarder
                current_file_name = self.tab_widget.tabText(index)
                if current_file_name.startswith('Tab '):
                    # C'est un nouveau fichier, on utilise Save As
                    self.tab_widget.setCurrentIndex(index)  # Switch to the tab being closed
                    self.save_as_file()
                else:
                    # C'est un fichier existant
                    try:
                        with open(current_file_name, "w") as file:
                            file.write(editor.toPlainText())
                    except Exception as e:
                        QMessageBox.warning(self, "Save Error", f"Could not save file: {str(e)}")
                        return  # Ne pas fermer l'onglet si la sauvegarde échoue

            elif reply == QMessageBox.Cancel:
                return  # Ne pas fermer l'onglet

        # Nettoyer les références dans tabs_data
        if tab in self.tabs_data:
            del self.tabs_data[tab]
        
        # Fermer l'onglet
        self.tab_widget.removeTab(index)

    def closeEvent(self, event):
        """Gère l'événement de fermeture de l'application."""
        # Vérifier tous les onglets non sauvegardés
        unsaved_tabs = []
        for i in range(self.tab_widget.count()):
            tab = self.tab_widget.widget(i)
            editor = tab.layout().itemAt(0).widget()
            if editor.document().isModified():
                unsaved_tabs.append((i, self.tab_widget.tabText(i)))

        if unsaved_tabs:
            # S'il y a des onglets non sauvegardés
            msg = QMessageBox()
            msg.setIcon(QMessageBox.Warning)
            msg.setWindowTitle("Unsaved Changes")
            
            if len(unsaved_tabs) == 1:
                msg.setText(f"The file '{unsaved_tabs[0][1]}' has unsaved changes.")
            else:
                msg.setText(f"There are {len(unsaved_tabs)} files with unsaved changes.")
            
            msg.setInformativeText("Do you want to save before closing?")
            msg.setStandardButtons(QMessageBox.Save | QMessageBox.Discard | QMessageBox.Cancel)
            msg.setDefaultButton(QMessageBox.Save)
            
            ret = msg.exec_()
            
            if ret == QMessageBox.Save:
                # Sauvegarder tous les fichiers non sauvegardés
                for tab_index, _ in unsaved_tabs:
                    self.tab_widget.setCurrentIndex(tab_index)
                    if self.tab_widget.tabText(tab_index).startswith('New Tab'):
                        self.save_as_file()
                    else:
                        self.save_file()
                    # Vérifier si la sauvegarde a été annulée
                    if self.tab_widget.widget(tab_index).layout().itemAt(0).widget().document().isModified():
                        event.ignore()
                        return
                event.accept()
            elif ret == QMessageBox.Discard:
                event.accept()
            else:  # Cancel
                event.ignore()
        else:
            self.save_file_history(os.path.abspath(self.tab_widget.tabText(self.tab_widget.currentIndex())))
            event.accept()

    def load_last_file(self):
        """Charge le dernier fichier édité s'il existe."""
        try:
            if os.path.exists(self.history_file):
                with open(self.history_file, "r") as f:
                    last_file = f.read().strip()
                    if last_file and os.path.exists(last_file):
                        with open(last_file, "r") as file:
                            current_tab = self.tab_widget.currentWidget()
                            editor = current_tab.layout().itemAt(0).widget()
                            editor.setPlainText(file.read())
                            self.tab_widget.setTabText(self.tab_widget.currentIndex(), os.path.basename(last_file))
                            editor.document().setModified(False)
                            self.update_window_title(self.tab_widget.currentIndex())
        except Exception as e:
            print(f"Error loading last file: {e}")

    def save_file_history(self, file_path):
        """Sauvegarde le chemin du fichier dans l'historique."""
        try:
            os.makedirs(os.path.dirname(self.history_file), exist_ok=True)
            with open(self.history_file, "w") as f:
                f.write(file_path)
        except Exception as e:
            print(f"Error saving file history: {e}")

class AnotherWindow(MyDrawppIDE):
    def __init__(self):
        super().__init__()

if __name__ == "__main__":
    app = QApplication(sys.argv)

    window = MyDrawppIDE()
    if platform.system() == "Windows":
        icon = QIcon(os.path.join(window.draw_folder, "IDE", "Dpp_circle.ico"))
        app.setWindowIcon(icon)
        if hasattr(QtWin, 'setCurrentProcessExplicitAppUserModelID'):
            myappid = 'mon.application.ide.version1.0'
            QtWin.setCurrentProcessExplicitAppUserModelID(myappid)

    window.show()
    sys.exit(app.exec_())