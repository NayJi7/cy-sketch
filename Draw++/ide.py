from PyQt5.QtWidgets import (QApplication, QMainWindow, QTextEdit, QVBoxLayout, QTabWidget,QPushButton, QFileDialog, QMessageBox, QWidget, QMenuBar, QAction, QToolBar, QPlainTextEdit, QSizePolicy, QSplitter, QTabBar)
from PyQt5.QtGui import QTextCharFormat, QColor, QSyntaxHighlighter, QTextCursor, QFont, QIcon,QPainter,QTextFormat
from PyQt5.QtCore import Qt, QProcess,QRect, QSize,QRegExp
import os
import platform
import subprocess
import sys
import psutil
from PyQt5.QtCore import QTimer
import re
if platform.system() == "Windows":
    from PyQt5.QtWinExtras import QtWin   #Pour Windows uniquement  #type: ignore 

# Ces 2 classes sont là pour mettre les numéros de ligne, pas important et juste 
# esthétique, rien à modifier dedans
class LineNumberArea(QWidget):
    def __init__(self, editor):
        super().__init__(editor)
        self.editor = editor  # Reference to the main editor

    def sizeHint(self):
        """Returns the width of the margin for line numbers."""
        return QSize(self.editor.line_number_area_width(), 0)

    def paintEvent(self, event):
        """Draws the line numbers."""
        self.editor.line_number_area_paint_event(event)

    def setToolTip(self, tooltip):
        """Set the tooltip for the line number area."""
        super().setToolTip(tooltip)  # Ensure the tooltip is set correctly

class CodeEditor(QPlainTextEdit):

    def __init__(self, parent=None):
        super().__init__(parent)
        self.lineNumberArea = LineNumberArea(self)
        self.highlighter = SyntaxHighlighter(self.document())  # Store highlighter reference

        self.blockCountChanged.connect(self.update_line_number_area_width)
        self.updateRequest.connect(self.update_line_number_area)
        self.cursorPositionChanged.connect(self.highlight_current_line)

        self.update_line_number_area_width(0)
        self.highlight_current_line()

        # Add timer for delayed syntax checking
        self.syntax_check_timer = QTimer(self)
        self.syntax_check_timer.setSingleShot(True)
        self.syntax_check_timer.timeout.connect(self.check_syntax)
        self.textChanged.connect(self.start_syntax_check_timer)

    def start_syntax_check_timer(self):
        """Start the timer for delayed syntax checking."""
        self.syntax_check_timer.start(1000)  # Check syntax after 1 second of no typing

    def check_syntax(self):
        """Check syntax of the current text and highlight errors."""
        from COMPILATOR.src.lexer import init_lexer, suggest_keyword
        from COMPILATOR.src.parser import init_parser
        from COMPILATOR.src.myast import execute_ast

        text = self.toPlainText()
        error_lines = set()

        # Get the terminal safely
        terminal = None
        try:
            current_tab = self.parent()
            while current_tab and not isinstance(current_tab, QWidget) or not hasattr(current_tab, 'layout') or not current_tab.layout():
                current_tab = current_tab.parent()
                if not current_tab:
                    break
            
            if current_tab and current_tab.layout():
                splitter = current_tab.layout().itemAt(0).widget()
                if splitter:
                    terminal_container = splitter.widget(1)
                    if terminal_container:
                        terminal = terminal_container.findChild(QPlainTextEdit)
        except Exception:
            pass  # Silently fail if we can't get the terminal

        output = ""  # Initialize output variable
        try:
            lexer = init_lexer()
            parser = init_parser()
            lexer.input(text)
            
            try:
                output = parser.parse(text, lexer=lexer)  # Capture output from parsing
                trad = execute_ast(output, False, "IDE")
                # Clear error highlighting if parsing succeeds
                if hasattr(self, 'highlighter'):
                    self.highlighter.set_error_lines(set())
            except SyntaxError as e:
                # Extract line number from error message
                error_msg = str(e)
                line_match = re.search(r'line (\d+)', error_msg)
                unexpected_match = re.search(r'unexpected end', error_msg)
                if line_match:
                    line_num = int(line_match.group(1))
                    # Add the line number to error lines
                    error_lines.add(line_num - 1)  # Convert to 0-based line number
                    
                    # Update the highlighter
                    if hasattr(self, 'highlighter'):
                        self.highlighter.set_error_lines(error_lines)
                        # Force a complete rehighlight
                        self.document().markContentsDirty(0, len(text))
                        self.highlighter.rehighlight()
                
                elif unexpected_match:
                    for i in range(lexer.lineno):
                        error_lines.add(i)
                    
                    # Update the highlighter
                    if hasattr(self, 'highlighter'):
                        self.highlighter.set_error_lines(error_lines)
                        # Force a complete rehighlight
                        self.document().markContentsDirty(0, len(text))
                        self.highlighter.rehighlight()

        except Exception as e:
            if terminal:
                cursor = terminal.textCursor()
                cursor.movePosition(QTextCursor.End)
                error_format = QTextCharFormat()
                error_format.setForeground(QColor("red"))
                cursor.insertText(f"-#red Error during syntax checking: {str(e)}\n", error_format)
                terminal.setTextCursor(cursor)
                terminal.ensureCursorVisible()

        # Handle translation errors specifically
        if "Error during the traduction of the main AST" in output:
            # Extract line number and message
            translation_error_match = re.search(r'Error during the traduction of the main AST : (.+)', output)
            if translation_error_match:
                translation_error_msg = translation_error_match.group(1)
                cursor.insertText(f"-#red {translation_error_msg}\n", error_format)
                terminal.setTextCursor(cursor)
                terminal.ensureCursorVisible()

        # Make sure the error highlighting is visible
        if error_lines:
            self.viewport().update()

    def keyPressEvent(self, event):
        # Handle Ctrl+C and Ctrl+X to copy/cut whole line if no text is selected
        if event.modifiers() == Qt.ControlModifier and event.key() in (Qt.Key_C, Qt.Key_X):
            cursor = self.textCursor()
            if not cursor.hasSelection():
                # Store original position
                original_position = cursor.position()
                # Select the current line without the newline character
                cursor.movePosition(QTextCursor.StartOfLine)
                cursor.movePosition(QTextCursor.EndOfLine, QTextCursor.KeepAnchor)
                selected_text = cursor.selectedText()
                
                # Add the line to clipboard without newline
                QApplication.clipboard().setText(selected_text)
                
                # For Ctrl+X, remove the entire line including newline
                if event.key() == Qt.Key_X:
                    cursor.movePosition(QTextCursor.StartOfLine)
                    cursor.movePosition(QTextCursor.Down, QTextCursor.KeepAnchor)
                    if cursor.atEnd():
                        cursor.movePosition(QTextCursor.EndOfLine, QTextCursor.KeepAnchor)
                    cursor.removeSelectedText()
                else:
                    # For Ctrl+C, restore cursor position
                    cursor.setPosition(original_position)
                    self.setTextCursor(cursor)
                return
        super().keyPressEvent(event)

    def line_number_area_width(self):
        """Calcule la largeur de la marge pour les numéros de ligne."""
        digits = len(str(self.blockCount()))
        space = 6 + self.fontMetrics().horizontalAdvance('9') * digits
        return space

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
                painter.drawText(0, int(top), self.lineNumberArea.width() - 3, self.fontMetrics().height() - 2,
                               Qt.AlignRight | Qt.AlignVCenter, number)

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
        self.error_lines = set()  # Store lines with syntax errors

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

        # Format pour les erreurs syntaxiques - Make it more visible
        self.error_format = QTextCharFormat()
        self.error_format.setUnderlineStyle(QTextCharFormat.WaveUnderline)
        self.error_format.setUnderlineColor(QColor(Qt.red))
        self.error_format.setForeground(QColor(Qt.red))  # Red text

        # Ajouter des règles
        self.add_rules(["var","func", "return"], self.keyword_format)
        self.add_rules(["draw circle", "draw line", "draw square", "draw rectangle", "draw triangle", "draw polygon", "draw ellipse", "draw arc", ], self.drawing_format)
        self.add_rules(["for", "while", "if", "else", "elif"], self.control_format)

    def add_rules(self, patterns, text_format):
        """Ajoute des règles de surlignage."""
        for pattern in patterns:
            # Utilisation des expressions régulières avec \b pour les mots entiers
            regex = QRegExp(rf"\b{pattern}\b")
            self.rules.append((regex, text_format))

    def set_error_lines(self, error_lines):
        """Met à jour les lignes contenant des erreurs syntaxiques."""
        self.error_lines = error_lines
        self.rehighlight()

    def highlightBlock(self, text):
        """Applique les règles de surlignage au bloc de texte donné."""
        # Get the actual line number (0-based)
        block_number = self.currentBlock().blockNumber()

        # Apply error underlining first if this block has an error
        if block_number in self.error_lines:
            self.setFormat(0, len(text), self.error_format)

        # Then apply syntax highlighting
        for regex, text_format in self.rules:
            index = regex.indexIn(text)
            while index >= 0:
                length = regex.matchedLength()
                # Create a combined format that preserves error underlining
                if block_number in self.error_lines:
                    combined_format = QTextCharFormat(text_format)
                    combined_format.setUnderlineStyle(self.error_format.underlineStyle())
                    combined_format.setUnderlineColor(self.error_format.underlineColor())
                    combined_format.setBackground(self.error_format.background())
                    self.setFormat(index, length, combined_format)
                else:
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
        self.setStyleSheet("""
            QMainWindow {
                background-color: #333333;
                color: white;
                font-family: Consolas;
            }
            QMessageBox {
                font-family: Consolas;
            }
            QMessageBox QPushButton {
                font-family: Consolas;
            }
            QFileDialog {
                font-family: Consolas;
            }
            QLabel {
                font-family: Consolas;
            }
            QPushButton {
                font-family: Consolas;
            }
        """)
        
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
                top: -1px;
                border-top: none;
            }}
            QTabBar::tab {{
                background: #2A2A2A;
                color: white;
                padding: 8px 15px;
                padding-right: 0px;
                padding-bottom: 12px;
                border: 1px solid #444444;
                border-bottom: none;
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
                font-family: Consolas;
            }
            QToolBar QToolButton {
                background-color: #1E1E1E;
                border: none;
                padding: 5px;
                color: white;
                font-family: Consolas;
            }
            QToolBar QToolButton:hover {
                background-color: #555555;
            }
            QToolBar QToolButton:pressed {
                background-color: #666666;
            }
            QToolBar QToolButton#runButton {
                background-color: #2E7D32;
                border-radius: 2px;
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
        open_action = QAction("Open", self)
        open_action.setShortcut("Ctrl+O")
        open_action.setToolTip("Open (Ctrl+O)")
        open_action.triggered.connect(self.open_file)
        open_action.triggered.connect(lambda: self.simulate_button_click(open_action))
        self.toolbar.addAction(open_action)

        # Séparateur
        separator1 = QWidget()
        separator1.setFixedWidth(1)
        separator1.setStyleSheet("background-color: #1E1E1E;")
        self.toolbar.addWidget(separator1)

        save_action = QAction("Save", self)
        save_action.setShortcut("Ctrl+S")
        save_action.setToolTip("Save (Ctrl+S)")
        save_action.triggered.connect(self.save_file)
        save_action.triggered.connect(lambda: self.simulate_button_click(save_action))
        self.toolbar.addAction(save_action)

        # Séparateur
        separator2 = QWidget()
        separator2.setFixedWidth(1)
        separator2.setStyleSheet("background-color: #1E1E1E;")
        self.toolbar.addWidget(separator2)

        save_as_action = QAction("Save As", self)
        save_as_action.setShortcut("Ctrl+Tab+S")
        save_as_action.setToolTip("Save As (Ctrl+Tab+S)")
        save_as_action.triggered.connect(self.save_as_file)
        save_as_action.triggered.connect(lambda: self.simulate_button_click(save_as_action))
        self.toolbar.addAction(save_as_action)

        # Séparateur
        separator3 = QWidget()
        separator3.setFixedWidth(1)
        separator3.setStyleSheet("background-color: #1E1E1E;")
        self.toolbar.addWidget(separator3)

        new_tab_action = QAction("New Tab", self)
        new_tab_action.setShortcut("Ctrl+T")
        new_tab_action.setToolTip("New Tab (Ctrl+T)")
        new_tab_action.triggered.connect(self.open_new_tab)
        new_tab_action.triggered.connect(lambda: self.simulate_button_click(new_tab_action))
        self.toolbar.addAction(new_tab_action)

        # Séparateur
        separator4 = QWidget()
        separator4.setFixedWidth(1)
        separator4.setStyleSheet("background-color: #1E1E1E;")
        self.toolbar.addWidget(separator4)

        new_window_action = QAction("New Window", self)
        new_window_action.setShortcut("Ctrl+N")
        new_window_action.setToolTip("New Window (Ctrl+N)")
        new_window_action.triggered.connect(self.show_new_window)
        new_window_action.triggered.connect(lambda: self.simulate_button_click(new_window_action))
        self.toolbar.addAction(new_window_action)

        # Séparateur
        separator5 = QWidget()
        separator5.setFixedWidth(1)
        separator5.setStyleSheet("background-color: #1E1E1E;")
        self.toolbar.addWidget(separator5)

        # Terminal toggle button
        terminal_action = QAction("Terminal", self)
        terminal_action.setShortcut("Ctrl+Alt+T")  # Common shortcut for terminal
        terminal_action.setToolTip("Terminal (Ctrl+Alt+T)")
        terminal_action.setCheckable(True)  # Make it checkable
        terminal_action.setChecked(False)  # Terminal is hidden by default
        terminal_action.triggered.connect(self.toggle_terminal)
        terminal_action.triggered.connect(lambda: self.simulate_button_click(terminal_action))
        self.toolbar.addAction(terminal_action)
        # Store reference to terminal action
        self.terminal_action = terminal_action

        # Ajouter un widget extensible pour pousser les boutons suivants à droite
        spacer = QWidget()
        spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        self.toolbar.addWidget(spacer)

        # Boutons de droite (exécution)
        run_action = QAction("Run", self)
        run_action.setShortcut("F5")
        run_action.setToolTip("Run (F5)")
        run_action.triggered.connect(self.run_code)
        run_action.triggered.connect(lambda: self.simulate_button_click(run_action))
        self.toolbar.addAction(run_action)
        # Définir l'ID pour le style du bouton Run
        run_button = self.toolbar.widgetForAction(run_action)
        if run_button:
            run_button.setObjectName("runButton")

        # Séparateur
        separator6 = QWidget()
        separator6.setFixedWidth(1)
        separator6.setStyleSheet("background-color: #1E1E1E;")
        self.toolbar.addWidget(separator6)

        debug_action = QAction("Debug", self)
        debug_action.setShortcut("F6")
        debug_action.setToolTip("Debug (F6)")
        debug_action.triggered.connect(self.debug_code)
        debug_action.triggered.connect(lambda: self.simulate_button_click(debug_action))
        self.toolbar.addAction(debug_action)

        # Séparateur
        separator7 = QWidget()
        separator7.setFixedWidth(1)
        separator7.setStyleSheet("background-color: #1E1E1E;")
        self.toolbar.addWidget(separator7)

        compile_action = QAction("Compile", self)
        compile_action.setShortcut("F7")
        compile_action.setToolTip("Compile (F7)")
        compile_action.triggered.connect(self.compile_code)
        compile_action.triggered.connect(lambda: self.simulate_button_click(compile_action))
        self.toolbar.addAction(compile_action)

        # Connecter le changement d'onglet pour mettre à jour le titre
        self.tab_widget.currentChanged.connect(self.update_window_title)
        # Ajoute un premier onglet par défaut
        self.open_new_tab()

        # Store references to actions
        self.run_action = run_action
        self.compile_action = compile_action 
        self.debug_action = debug_action

        # Add close tab shortcut
        close_tab_action = QAction("Close Tab", self)
        close_tab_action.setShortcut("Ctrl+W")
        close_tab_action.triggered.connect(lambda: self.close_tab(self.tab_widget.currentIndex()))
        self.addAction(close_tab_action)

    def open_new_tab(self):
        """Créer un nouvel onglet avec un éditeur et un terminal."""
        print("Création d'un nouvel onglet")

        # Crée un nouvel onglet et son layout
        new_tab = QWidget()
        layout = QVBoxLayout()
        layout.setContentsMargins(0, 0, 0, 0)  # Remove margins
        layout.setSpacing(0)  # Remove spacing between widgets

        # Create a splitter
        splitter = QSplitter(Qt.Vertical)
        splitter.setChildrenCollapsible(False)  # Prevent widgets from being collapsed to 0
        splitter.setStyleSheet("""
            QSplitter {
                background-color: #333333;
            }
            QSplitter::handle {
                background-color: #444444;
                height: 2px;
            }
            QSplitter::handle:hover {
                background-color: #666666;
            }
        """)

        # Création de l'éditeur
        editor = CodeEditor(self)
        editor.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        editor.setStyleSheet("""
            QPlainTextEdit {
                background-color: #1E1E1E;
                color: #D4D4D4;
                font-family: Consolas;
                font-size: 18px;
                border: none;
                border-top: 1px solid #444444;
                padding: 5px;
            }
            QScrollBar:vertical {
                border: none;
                background: #2E2E2E;
                width: 10px;
                margin: 2px;
                border-radius: 5px;
            }
            QScrollBar::handle:vertical {
                background: #555555;
                min-height: 20px;
                border-radius: 5px;
                margin: 2px;
                width: 6px;
            }
            QScrollBar::handle:vertical:hover {
                background: #666666;
            }
            QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
                height: 0px;
            }
            QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
                background: none;
            }
            QScrollBar:horizontal {
                border: none;
                background: #2E2E2E;
                height: 10px;
                margin: 2px;
                border-radius: 5px;
            }
            QScrollBar::handle:horizontal {
                background: #555555;
                min-width: 20px;
                border-radius: 5px;
                margin: 2px;
                height: 6px;
            }
            QScrollBar::handle:horizontal:hover {
                background: #666666;
            }
            QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
                width: 0px;
            }
            QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {
                background: none;
            }
        """)
        editor.setTabStopDistance(4 * editor.fontMetrics().horizontalAdvance(' '))
        editor.document().setModified(False)

        # Ajouter le surlignage syntaxique
        highlighter = SyntaxHighlighter(editor.document())

        # Création du terminal avec bouton Clear intégré
        terminal_container = QWidget()
        terminal_container.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        terminal_layout = QVBoxLayout(terminal_container)
        terminal_layout.setContentsMargins(0, 0, 0, 0)
        terminal_layout.setSpacing(0)

        terminal = QPlainTextEdit(self)
        terminal.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        terminal.setReadOnly(True)
        terminal.setStyleSheet("""
            QPlainTextEdit {
                background-color: #000000;
                color: white;
                font-family: Consolas;
                font-size: 14px;
                border: none;
                border-top: 1px solid #444444;
                padding: 5px;
            }
            QScrollBar:vertical {
                border: none;
                background: #2E2E2E;
                width: 10px;
                margin: 2px;
                border-radius: 5px;
            }
            QScrollBar::handle:vertical {
                background: #555555;
                min-height: 20px;
                border-radius: 5px;
                margin: 2px;
                width: 6px;
            }
            QScrollBar::handle:vertical:hover {
                background: #666666;
            }
            QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
                height: 0px;
            }
            QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
                background: none;
            }
            QScrollBar:horizontal {
                border: none;
                background: #2E2E2E;
                height: 10px;
                margin: 2px;
                border-radius: 5px;
            }
            QScrollBar::handle:horizontal {
                background: #555555;
                min-width: 20px;
                border-radius: 5px;
                margin: 2px;
                height: 6px;
            }
            QScrollBar::handle:horizontal:hover {
                background: #666666;
            }
            QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
                width: 0px;
            }
            QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {
                background: none;
            }
        """)

        # Création du bouton Clear flottant
        clear_button = QPushButton("Clear", terminal)
        clear_button.setStyleSheet("""
            QPushButton {
                background-color: #333333;
                color: white;
                border: 1px solid #444444;
                padding: 5px 10px;
                border-radius: 2px;
                font-family: Consolas;
                position: absolute;
                right: 10px;
                top: 10px;
            }
            QPushButton:hover {
                background-color: #444444;
            }
            QPushButton:pressed {
                background-color: #555555;
            }
        """)
        clear_button.setFixedWidth(80)
        clear_button.setToolTip("Clear Terminal (Ctrl+Alt+C)")
        clear_button.clicked.connect(terminal.clear)

        # Add global shortcut for clear
        clear_action = QAction("Clear Terminal", self)
        clear_action.setShortcut("Ctrl+Alt+C")
        clear_action.setToolTip("Clear Terminal (Ctrl+Alt+C)")
        clear_action.triggered.connect(terminal.clear)
        self.addAction(clear_action)

        # Positionner le bouton dans le coin supérieur droit du terminal
        terminal.resizeEvent = lambda e: clear_button.move(
            terminal.width() - clear_button.width() - 25,
            10
        )

        terminal_layout.addWidget(terminal)

        # Add widgets to splitter
        splitter.addWidget(editor)
        splitter.addWidget(terminal_container)
        
        # Set initial sizes to 50-50
        splitter.setSizes([500, 500])

        # Add splitter to layout
        layout.addWidget(splitter)

        new_tab.setLayout(layout)

        if not hasattr(self, 'tabs_data'):
            self.tabs_data = {}
        self.tabs_data[new_tab] = {'editor': editor, 'terminal': terminal, 'highlighter': highlighter}

        # Ajouter l'onglet 
        tabName = f'New Tab'
        index = self.tab_widget.addTab(new_tab, tabName)
        self.tab_widget.setCurrentWidget(new_tab)
        
        # Set tooltip for the close button
        close_button = self.tab_widget.tabBar().tabButton(index, QTabBar.RightSide)
        if close_button:
            close_button.setToolTip("Close Tab (Ctrl+W)")
            close_button.setStyleSheet("""
                QPushButton {
                    margin: 2px;
                }
                QPushButton:hover {
                    background: #666666;
                    border-radius: 2px;
                }
            """)

        # After creating the terminal, set its visibility based on terminal action state
        if hasattr(self, 'terminal_action'):
            terminal_container.setVisible(self.terminal_action.isChecked())
            if not self.terminal_action.isChecked():
                splitter.setSizes([1, 0])

    def update_window_title(self, index):
        """Met à jour le titre de la fenêtre selon l'onglet actif."""
        if index == -1:
            self.setWindowTitle("DrawStudioCode")  # Aucun onglet actif
        else:
            current_tab_title = self.tab_widget.tabText(index)
            self.setWindowTitle(f"DrawStudioCode - {current_tab_title}")


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
                splitter = current_tab.layout().itemAt(0).widget()
                editor = splitter.widget(0)  # First widget in splitter is the editor
            else:
                # Create new tab
                self.open_new_tab()
                current_tab = self.tab_widget.currentWidget()
                splitter = current_tab.layout().itemAt(0).widget()
                editor = splitter.widget(0)  # First widget in splitter is the editor
            
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
            # Get editor from splitter's first widget
            splitter = current_tab.layout().itemAt(0).widget()
            editor = splitter.widget(0)  # First widget in splitter is the editor
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
            # Get editor from splitter's first widget
            splitter = current_tab.layout().itemAt(0).widget()
            editor = splitter.widget(0)  # First widget in splitter is the editor
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

        splitter = current_tab.layout().itemAt(0).widget()
        editor = splitter.widget(0)  # First widget in splitter is the editor
        terminal = tab_data['terminal']

        # Make sure terminal is visible
        if not terminal.isVisible():
            self.toggle_terminal()

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
        if platform.system() != "Windows":
            # On Unix systems, make the process a group leader
            if hasattr(process, 'setProcessGroup'):
                process.setProcessGroup(QProcess.MergeProcessGroup)
        tab_data['process'] = process  # Stocker le processus dans le dictionnaire

        # Définir le répertoire de travail sur le dossier Draw++
        process.setWorkingDirectory(draw_folder)

        # Connecter les sorties au terminal actif
        process.readyReadStandardOutput.connect(lambda: self.display_output(process, terminal))
        process.finished.connect(lambda: self.on_process_finished(current_tab))

        # Disable run buttons and change appearance
        self.disable_run_buttons()
        run_button = self.toolbar.widgetForAction(self.run_action)
        if run_button:
            self.run_action.setText("Stop")  # Update action text
            run_button.setText("Stop")       # Update button text
            run_button.setToolTip("Stop")
            run_button.setStyleSheet("""
                QToolButton {
                    background-color: #B71C1C !important;
                    color: white;
                    border: none;
                    border-radius: 2px;
                    padding: 5px;
                }
                QToolButton:hover {
                    background-color: #D32F2F !important;
                }
                QToolButton:pressed {
                    background-color: #C62828 !important;
                }
            """)
            # Enable the run button to act as a stop button
            self.run_action.setEnabled(True)
            # Disconnect any existing connections
            try:
                self.run_action.triggered.disconnect()
            except:
                pass
            # Connect to the stop function
            self.run_action.triggered.connect(lambda: self.stop_process(current_tab))

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

    def stop_process(self, tab):
        """Stops the running process for the given tab, including any child processes."""
        if tab in self.tabs_data:
            process = self.tabs_data[tab].get('process')
            if process and process.state() == QProcess.Running:
                pid = process.processId()
                print(f"Stopping process for tab {tab}, PID: {pid}")
                
                try:
                    if platform.system() == "Windows":
                        # Use taskkill to terminate the process and its children on Windows
                        subprocess.run(
                            ['taskkill', '/F', '/T', '/PID', str(pid)],
                            capture_output=True,
                            check=True
                        )
                    else:
                        # On Unix-like systems, terminate the process tree
                        kill_process_tree(pid)
                    
                    # Check if the process is still running
                    if is_process_running(pid):
                        print(f"Process {pid} did not terminate. Manual intervention may be required.")
                    else:
                        print(f"Process {pid} has been successfully stopped.")
                
                except Exception as e:
                    print(f"Error while stopping process {pid}: {e}")
                finally:
                    print(f"Process handling for tab {tab} completed.")
            else:
                print(f"No running process found for tab: {tab}")
                
        # Reset the run button
        run_button = self.toolbar.widgetForAction(self.run_action)
        if run_button:
            self.run_action.setText("Run")   # Update action text
            run_button.setText("Run")        # Update button text
            run_button.setStyleSheet("")
            # Reconnect the run action to run_code
            try:
                self.run_action.triggered.disconnect()
            except:
                pass
            self.run_action.triggered.connect(self.run_code)
        
        # Re-enable all buttons
        self.enable_run_buttons()

    def on_process_finished(self, tab):
        """Called when a process finishes running"""
        # Re-enable run buttons and restore appearance
        run_button = self.toolbar.widgetForAction(self.run_action)
        if run_button:
            self.run_action.setText("Run")   # Update action text
            run_button.setText("Run")        # Update button text
            run_button.setStyleSheet("")
            # Reconnect the run action to run_code
            try:
                self.run_action.triggered.disconnect()
            except:
                pass
            self.run_action.triggered.connect(self.run_code)
        
        self.enable_run_buttons()

    def disable_run_buttons(self):
        """Disable all run-related buttons"""
        self.run_action.setEnabled(False)
        self.compile_action.setEnabled(False)
        self.debug_action.setEnabled(False)

    def enable_run_buttons(self):
        """Enable all run-related buttons"""
        self.run_action.setEnabled(True)
        self.compile_action.setEnabled(True)
        self.debug_action.setEnabled(True)

    def run_code(self):
        """Exécute le code en mode normal."""
        self.execute_code("run")

    def compile_code(self):
        """Compile le code sans l'exécuter."""
        self.execute_code("compile")

    def debug_code(self):
        """Exécute le code en mode debug."""
        # Show terminal if it's hidden
        current_tab = self.tab_widget.currentWidget()
        if current_tab:
            splitter = current_tab.layout().itemAt(0).widget()
            terminal_container = splitter.widget(1)
            if not terminal_container.isVisible():
                self.terminal_action.setChecked(True)
                self.toggle_terminal()
        
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

        # Get editor from splitter's first widget
        splitter = tab.layout().itemAt(0).widget()
        editor = splitter.widget(0)  # First widget in splitter is the editor
        
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
                if current_file_name.startswith('New Tab'):
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
            splitter = tab.layout().itemAt(0).widget()
            editor = splitter.widget(0)  # First widget in splitter is the editor
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
                    tab = self.tab_widget.widget(tab_index)
                    splitter = tab.layout().itemAt(0).widget()
                    editor = splitter.widget(0)  # First widget in splitter is the editor
                    if editor.document().isModified():
                        event.ignore()
                        return
                event.accept()
            elif ret == QMessageBox.Discard:
                event.accept()
            else:  # Cancel
                event.ignore()
        else:
            # Try to get the full path from the history
            try:
                with open(self.history_file, "r") as f:
                    last_file = f.read().strip()
                if last_file and os.path.exists(last_file):
                    self.save_file_history(last_file)
            except:
                pass
            event.accept()

    def load_last_file(self):
        """Charge le dernier fichier édité s'il existe."""
        try:
            if os.path.exists(self.history_file):
                with open(self.history_file, "r") as f:
                    last_file = f.read().strip()
                    if last_file and os.path.exists(last_file) and not last_file.startswith('New Tab'):
                        try:
                            with open(last_file, "r") as file:
                                current_tab = self.tab_widget.currentWidget()
                                # Get editor from splitter's first widget
                                splitter = current_tab.layout().itemAt(0).widget()
                                editor = splitter.widget(0)  # First widget in splitter is the editor
                                editor.setPlainText(file.read())
                                self.tab_widget.setTabText(self.tab_widget.currentIndex(), os.path.basename(last_file))
                                editor.document().setModified(False)
                                self.update_window_title(self.tab_widget.currentIndex())
                        except Exception as e:
                            print(f"Error reading last file {last_file}: {e}")
                            # If we can't read the last file, just start with a new tab
                            pass
        except Exception as e:
            print(f"Error loading history file: {e}")

    def save_file_history(self, file_path):
        """Sauvegarde le chemin du fichier dans l'historique."""
        try:
            # Only save if it's a real file path and not a new tab
            if file_path and os.path.exists(file_path) and not file_path.endswith('New Tab'):
                os.makedirs(os.path.dirname(self.history_file), exist_ok=True)
                with open(self.history_file, "w") as f:
                    f.write(file_path)
        except Exception as e:
            print(f"Error saving file history: {e}")

    def simulate_button_click(self, action):
        """Simulates a button click animation when a keyboard shortcut is used"""
        button = self.toolbar.widgetForAction(action)
        if button:
            # Save original style
            original_style = button.styleSheet()
            
            # Apply pressed style with transition
            button.setStyleSheet("""
                QToolButton {
                    background-color: #666666;
                }
            """)
            
            # Reset style after 150ms with a longer duration for better visibility
            QTimer.singleShot(150, lambda: button.setStyleSheet(original_style))

    def toggle_terminal(self):
        """Toggles the visibility of the terminal in the current tab."""
        current_tab = self.tab_widget.currentWidget()
        if not current_tab:
            return

        splitter = current_tab.layout().itemAt(0).widget()
        terminal_container = splitter.widget(1)  # Terminal is the second widget in splitter

        if terminal_container.isVisible():
            # Save current sizes before hiding
            self.last_sizes = splitter.sizes()
            terminal_container.hide()
            # Give all space to editor
            splitter.setSizes([1, 0])
        else:
            terminal_container.show()
            if hasattr(self, 'last_sizes'):
                # Restore previous sizes
                splitter.setSizes(self.last_sizes)
            else:
                # Default 50-50 split
                splitter.setSizes([500, 500])

        # Update button state
        self.terminal_action.setChecked(terminal_container.isVisible())

def is_process_running(pid):
    """Check if a process is still running."""
    try:
        process = psutil.Process(pid)
        return process.is_running()
    except psutil.NoSuchProcess:
        return False

def kill_process_tree(pid):
    """Kill a process and its entire tree (children and grandchildren)."""
    try:
        parent = psutil.Process(pid)
        # Terminate child processes first
        for child in parent.children(recursive=True):
            print(f"Terminating child process PID: {child.pid}")
            child.terminate()
        parent.terminate()  # Terminate the parent process
        
        # Wait for all processes to exit
        gone, alive = psutil.wait_procs([parent] + parent.children(), timeout=3)
        if alive:
            print(f"Forcing kill for remaining processes: {[p.pid for p in alive]}")
            for proc in alive:
                proc.kill()
        print(f"Successfully terminated process tree for PID {pid}")
    except psutil.NoSuchProcess:
        print(f"Process {pid} no longer exists.")
    except Exception as e:
        print(f"Error while killing process tree for PID {pid}: {e}")

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