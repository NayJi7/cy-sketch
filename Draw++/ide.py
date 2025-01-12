from PyQt5.QtWidgets import (QApplication, QMainWindow, QTextEdit, QVBoxLayout, QTabWidget,QPushButton, QFileDialog, QMessageBox, QWidget, QMenuBar, QAction, QToolBar, QPlainTextEdit, QSizePolicy, QSplitter, QTabBar, QToolTip, QLineEdit, QHBoxLayout, QCheckBox)
from PyQt5.QtGui import QTextCharFormat, QColor, QSyntaxHighlighter, QTextCursor, QFont, QIcon,QPainter,QTextFormat, QFontMetricsF, QTextBlockUserData, QTextDocument
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


####################### FIRST BIG PART ###########################""

# These 2 classes are here to display line numbers, not important and just 
# aesthetic, nothing to modify inside
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

        # Set up tab handling
        self.setTabStopDistance(QFontMetricsF(self.font()).horizontalAdvance(' ') * 4)  # 4 spaces per tab

        self.setMouseTracking(True)  # Enable mouse tracking for tooltips

        # Add search functionality
        self.last_search = ""
        self.case_sensitive = False

    def start_syntax_check_timer(self):
        """Start the timer for delayed syntax checking."""
        self.syntax_check_timer.start(100)  # Check syntax after 1 second of no typing

    def _handle_error(self, error_msg, error_lines, error_messages):
        """Handle error message parsing and immediate UI update."""
        from COMPILATOR.src.lexer import suggest_keyword

        # First try to find line number in standard format
        line_match = re.search(r'at line (\d+)', error_msg)
        if not line_match:
            # Try to find line number in AST translation error format
            if "ValueError : Variable" in error_msg and "is used before initialization" in error_msg:
                # Extract variable name from error message
                var_match = re.search(r"Variable '(\w+)' is used", error_msg)
                if var_match:
                    var_name = var_match.group(1)
                    text = self.toPlainText()
                    lines = text.splitlines()
                    # Look for the variable as a standalone token
                    for i, line in enumerate(lines):
                        # Use regex to find the variable as a standalone word
                        if re.search(rf'\b{var_name}\b', line):
                            line_num = i
                            error_lines.add(line_num)
                            break
                    else:  # If no match found
                        # If no line number found, mark the entire document as having an error
                        for block_num in range(self.document().blockCount()):
                            error_lines.add(block_num)
                        line_num = 0
            else:
                # If no line number found, mark the entire document as having an error
                for block_num in range(self.document().blockCount()):
                    error_lines.add(block_num)
                line_num = 0  # Use first line for the error message
        else:
            line_num = int(line_match.group(1)) - 1
            error_lines.add(line_num)
            
        suggestion = None
        
        # Handle unknown identifier suggestions
        if "Unknown identifier" in error_msg:
            unknown_id = error_msg.split("'")[1].split("'")[0]
            suggestion_word = suggest_keyword(unknown_id)
            if suggestion_word:
                suggestion = f"Did you mean '{suggestion_word}'?"
            else:
                suggestion = f"Check if you initialized correctly all your variables (with var) and functions (with func)"
        
        # Handle expected value suggestions
        elif "expected" in error_msg:
            expected_word = error_msg.split("expected ")[1].split(" ")[0]
            suggestion_word = suggest_keyword(expected_word)
            if suggestion_word:
                suggestion = f"Change the value of the parameter to {suggestion_word}"

        # Handle type error suggestions
        elif "TypeError" in error_msg:
            if "is not long enough" in error_msg:
                suggestion = "Increase the size of your char array"
            elif "cannot compare" in error_msg:
                suggestion = "Use numeric types for comparisons"
            elif "cannot be float" in error_msg:
                suggestion = "Use an integer value instead"
            elif "trying to assign" in error_msg:
                suggestion = "Make sure the types match in your assignment"
            elif "is not a supported color" in error_msg:
                color = error_msg.split('"')[1]
                suggestion_word = suggest_keyword(color)
                if suggestion_word:
                    suggestion = f"Did you mean '{suggestion_word}'?"
                else:
                    suggestion = "Use one of the supported colors"

        # Handle value error suggestions
        elif "ValueError" in error_msg:
            if "not initialized" in error_msg or "used before initialization" in error_msg:
                var_name = error_msg.split("'")[1].split("'")[0]
                suggestion = f"Initialize the variable '{var_name}' with var before using it"
                # Clean up the error message for AST translation errors
                if "Error during the traduction of the main AST" in error_msg:
                    error_msg = f"Variable '{var_name}' is used before initialization. Initialize it with var"
            elif "function not initialized" in error_msg:
                func_name = error_msg.split("'")[1]
                suggestion = f"Define the function '{func_name}' with func before using it"

        # Handle index error suggestions
        elif "IndexError" in error_msg:
            if "requires" in error_msg and "arguments" in error_msg:
                suggestion = "Check the number of arguments in your function call"
            elif "is not long enough" in error_msg:
                suggestion = "The array size is too small for the string"

        # Handle name error suggestions
        elif "NameError" in error_msg:
            if "can only change" in error_msg:
                suggestion = "You can only use 'cursor' or 'window' here"

        # Handle syntax error suggestions
        elif "Syntax error" in error_msg:
            if "unexpected end" in error_msg:
                suggestion = "Check for missing closing brackets or parentheses"
            elif "parenthesis" in error_msg:
                suggestion = "Ensure parentheses '(' and ')' are balanced"
            elif "braces" in error_msg:
                suggestion = "Check that all braces '{' and '}' are properly matched"
            elif "semicolon" in error_msg:
                suggestion = "Remove unnecessary semicolons"
            elif "comparison" in error_msg:
                suggestion = "Verify the comparison expression and its operands"
            elif "comma" in error_msg:
                suggestion = "Check parameter separation in function calls"

        elif "Lexical error" in error_msg:  
            if "Illegal character" in error_msg:
                char = error_msg.split("'")[1].split("'")[0]
                suggestion = (f"Suggestion: Remove the '{char}' character or replace it with a valid one.")

        # Handle critical error suggestions
        elif "CriticalError" in error_msg:
            if "Unsupported node" in error_msg:
                suggestion = "There's an error in your code structure"

        error_messages[line_num] = {
            'error': error_msg,
            'suggestion': suggestion
        }
        
        if hasattr(self, 'highlighter'):
            self.highlighter.set_error_lines(error_lines)
            self.highlighter.error_messages = error_messages

    def check_syntax(self):
        """Check syntax of the current text and highlight/underline errors."""
        from COMPILATOR.src.lexer import init_lexer, suggest_keyword
        from COMPILATOR.src.parser import init_parser
        from COMPILATOR.src.myast import execute_ast
        import sys
        from io import StringIO

        # Capture stdout to prevent printing success message
        old_stdout = sys.stdout
        sys.stdout = StringIO()

        text = self.toPlainText()
        error_lines = set()
        error_messages = {}

        try:
            # Lexical Analysis
            lexer = init_lexer()
            lexer.input(text)
            try:
                list(lexer)  # Try to tokenize
            except SyntaxError as e:
                sys.stdout = old_stdout  # Restore stdout
                self._handle_error(str(e), error_lines, error_messages)
                return

            # Reset lexer for parsing
            lexer = init_lexer()
            parser = init_parser()
            lexer.input(text)
            
            try:
                ast = parser.parse(text, lexer=lexer)
                if ast:
                    execute_ast(ast, False, "IDE")
                if hasattr(self, 'highlighter'):
                    self.highlighter.set_error_lines(set())
            except Exception as e:
                self._handle_error(str(e), error_lines, error_messages)

        except Exception as e:
            print(f"Error during syntax check: {e}")

        finally:
            sys.stdout = old_stdout  # Restore stdout

        # Update error highlighting
        if hasattr(self, 'highlighter'):
            self.highlighter.set_error_lines(error_lines)
            self.highlighter.error_messages = error_messages

    def keyPressEvent(self, event):
        # Handle Alt+Up/Down to move lines
        if event.modifiers() == Qt.AltModifier and event.key() in (Qt.Key_Up, Qt.Key_Down):
            cursor = self.textCursor()
            document = self.document()
            
            # Get current block number and total blocks
            current_block = cursor.blockNumber()
            total_blocks = document.blockCount()
            
            # Check if movement is possible
            if (event.key() == Qt.Key_Up and current_block <= 0) or \
               (event.key() == Qt.Key_Down and current_block >= total_blocks - 1):
                return
            
            # Begin edit block for undo/redo
            cursor.beginEditBlock()
            
            try:
                # Store original cursor position within the line
                original_pos = cursor.positionInBlock()
                
                # Select and store current line
                cursor.movePosition(QTextCursor.StartOfBlock)
                cursor.movePosition(QTextCursor.EndOfBlock, QTextCursor.KeepAnchor)
                current_line = cursor.selectedText()
                
                # Move to the target line (up or down)
                if event.key() == Qt.Key_Up:
                    cursor.movePosition(QTextCursor.StartOfBlock)
                    cursor.movePosition(QTextCursor.Up)
                else:
                    cursor.movePosition(QTextCursor.StartOfBlock)
                    cursor.movePosition(QTextCursor.Down)
                
                # Select and store target line
                cursor.movePosition(QTextCursor.EndOfBlock, QTextCursor.KeepAnchor)
                target_line = cursor.selectedText()
                
                # Replace target line with current line
                cursor.insertText(current_line)
                
                # Move back and replace original line with target line
                if event.key() == Qt.Key_Up:
                    cursor.movePosition(QTextCursor.Down)
                else:
                    cursor.movePosition(QTextCursor.Up)
                
                cursor.movePosition(QTextCursor.StartOfBlock)
                cursor.movePosition(QTextCursor.EndOfBlock, QTextCursor.KeepAnchor)
                cursor.insertText(target_line)
                
                # Move cursor to the moved line and restore position within line
                if event.key() == Qt.Key_Up:
                    cursor.movePosition(QTextCursor.Up)
                else:
                    cursor.movePosition(QTextCursor.Down)
                
                cursor.movePosition(QTextCursor.StartOfBlock)
                cursor.movePosition(QTextCursor.Right, QTextCursor.MoveAnchor, min(original_pos, len(target_line)))
                
            finally:
                cursor.endEditBlock()
                self.setTextCursor(cursor)
            return

        # Handle Ctrl+D to duplicate current line
        if event.modifiers() == Qt.ControlModifier and event.key() == Qt.Key_D:
            cursor = self.textCursor()
            # Store original position
            original_position = cursor.position()
            original_anchor = cursor.anchor()
            
            # If text is selected, duplicate the selection
            if cursor.hasSelection():
                start = cursor.selectionStart()
                end = cursor.selectionEnd()
                text = cursor.selectedText()
                cursor.setPosition(end)
                cursor.insertText("\n" + text)
                # Move to duplicated selection
                new_start = end + 1
                new_end = new_start + len(text)
                cursor.setPosition(new_start)
                cursor.setPosition(new_end, QTextCursor.KeepAnchor)
            else:
                # Get current line text
                cursor.movePosition(QTextCursor.StartOfLine)
                line_start = cursor.position()
                cursor.movePosition(QTextCursor.EndOfLine, QTextCursor.KeepAnchor)
                text = cursor.selectedText()
                
                # Calculate relative position in the line
                relative_pos = original_position - line_start
                
                # Insert duplicated line
                cursor.movePosition(QTextCursor.EndOfLine)
                cursor.insertText("\n" + text)
                
                # Move cursor to duplicated line
                cursor.movePosition(QTextCursor.StartOfLine)
                cursor.movePosition(QTextCursor.Right, QTextCursor.MoveAnchor, relative_pos)
            
            self.setTextCursor(cursor)
            return

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

        # Auto-closing pairs
        pairs = {
            '(': ')',
            '[': ']',
            '{': '}',
            '"': '"',
        }
        if event.text() in pairs:
            cursor = self.textCursor()
            self.insertPlainText(event.text() + pairs[event.text()])
            cursor.movePosition(QTextCursor.Left, QTextCursor.MoveAnchor, 1)
            self.setTextCursor(cursor)
            return
        # Skip over closing character if it's already there
        elif event.text() in pairs.values():
            cursor = self.textCursor()
            next_char = self.document().characterAt(cursor.position())
            if next_char == event.text():
                cursor.movePosition(QTextCursor.Right, QTextCursor.MoveAnchor, 1)
                self.setTextCursor(cursor)
                return

        # Handle tab key
        elif event.key() == Qt.Key_Tab:
            # Insert 4 spaces instead of tab
            self.insertPlainText('    ')
            return
        # Handle auto-indentation
        elif event.key() == Qt.Key_Return or event.key() == Qt.Key_Enter:
            cursor = self.textCursor()
            block = cursor.block()
            text = block.text()
            
            # Get the indentation of the current line
            indentation = ''
            for char in text:
                if char.isspace():
                    indentation += char
                else:
                    break
            
            # Insert new line with the same indentation
            super().keyPressEvent(event)
            self.insertPlainText(indentation)
            return
            
        super().keyPressEvent(event)

    def line_number_area_width(self):
        """Calculate the width of the margin for line numbers."""
        digits = len(str(self.blockCount()))
        space = 6 + self.fontMetrics().horizontalAdvance('9') * digits
        return space

    def update_line_number_area_width(self, _):
        """Update the width of the margin for line numbers."""
        self.setViewportMargins(self.line_number_area_width(), 0, 0, 0)

    def update_line_number_area(self, rect, dy):
        """Redraw the line numbers after scrolling or modification."""
        if dy:
            self.lineNumberArea.scroll(0, dy)
        else:
            self.lineNumberArea.update(0, rect.y(), self.lineNumberArea.width(), rect.height())

        if rect.contains(self.viewport().rect()):
            self.update_line_number_area_width(0)

    def resizeEvent(self, event):
        """Adjust the line number area when resizing the editor."""
        super().resizeEvent(event)
        cr = self.contentsRect()
        self.lineNumberArea.setGeometry(QRect(cr.left(), cr.top(), self.line_number_area_width(), cr.height()))

    def line_number_area_paint_event(self, event):
        """Draw the line numbers."""
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
        """Highlight the current line."""
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

    def mouseMoveEvent(self, event):
        """Handle mouse movement to show tooltips on error lines"""
        super().mouseMoveEvent(event)
        cursor = self.cursorForPosition(event.pos())
        block = cursor.block()
        data = block.userData()
        
        if data and isinstance(data, TextBlockData):
            QToolTip.showText(event.globalPos(), data.tooltip)
        else:
            QToolTip.hideText()

    def find_text(self, text, forward=True, case_sensitive=False):
        """Find text in the editor."""
        if not text:
            return False

        # Store search settings
        self.last_search = text
        self.case_sensitive = case_sensitive

        # Get cursor for searching
        cursor = self.textCursor()
        document = self.document()

        # Create find flags
        flags = QTextDocument.FindFlags()
        if not forward:
            flags |= QTextDocument.FindBackward
        if case_sensitive:
            flags |= QTextDocument.FindCaseSensitively

        # If we have a selection, start from the appropriate end
        if cursor.hasSelection():
            if forward:
                cursor.setPosition(cursor.selectionEnd())
            else:
                cursor.setPosition(cursor.selectionStart())

        # Find the text
        cursor = document.find(text, cursor, flags)
        
        # If not found from cursor, try from start/end of document
        if cursor.isNull():
            cursor = self.textCursor()
            if forward:
                cursor.movePosition(QTextCursor.Start)
            else:
                cursor.movePosition(QTextCursor.End)
            cursor = document.find(text, cursor, flags)

        # If found, select it
        if not cursor.isNull():
            self.setTextCursor(cursor)
            return True
        return False

    def replace_text(self, find_text, replace_with, case_sensitive=False):
        """Replace selected text if it matches find_text."""
        cursor = self.textCursor()
        if cursor.hasSelection():
            selected_text = cursor.selectedText()
            if (case_sensitive and selected_text == find_text) or \
               (not case_sensitive and selected_text.lower() == find_text.lower()):
                cursor.insertText(replace_with)
                return True
        return False

    def replace_all_text(self, find_text, replace_with, case_sensitive=False):
        """Replace all occurrences of find_text with replace_with."""
        cursor = self.textCursor()
        cursor.beginEditBlock()  # Group all replacements as one undo operation
        
        # Start from beginning
        cursor.movePosition(QTextCursor.Start)
        self.setTextCursor(cursor)
        
        count = 0
        while self.find_text(find_text, True, case_sensitive):
            if self.replace_text(find_text, replace_with, case_sensitive):
                count += 1
        
        cursor.endEditBlock()
        return count



# This class is a pre-made class from PyQt5 forums to add line number effects in the IDE.
# It provides visual effects like line counting, syntax highlighting, and error highlighting.
class TextBlockData(QTextBlockUserData):
    def __init__(self, tooltip):
        super().__init__()
        self.tooltip = tooltip


####################### SECOND BIG PART ###########################""

# Class for highlighting terminal words in the language (return, from, if, cursor...)
# TBD <to be updated with all words as modified due to tabs addition causing crashes>
class SyntaxHighlighter(QSyntaxHighlighter):
    def __init__(self, document):
        super().__init__(document)
        self.rules = []
        self.error_lines = set()  # Store lines with syntax errors
        self.error_messages = {}  # Store error messages

        # Define formats
        self.comment_format = QTextCharFormat()
        self.comment_format.setForeground(QColor("#808080"))  # Grey color for comments
        self.comment_format.setFontItalic(True)  # Make comments italic
        
        # Store comment states
        self.multiLineCommentFormat = QTextCharFormat()
        self.multiLineCommentFormat.setForeground(QColor("#808080"))
        self.multiLineCommentFormat.setFontItalic(True)
        
        self.commentStartExpression = QRegExp("/\\*")
        self.commentEndExpression = QRegExp("\\*/")
        
        self.keyword_format = QTextCharFormat()
        self.keyword_format.setForeground(QColor("#ff5733"))  # Orange
        self.keyword_format.setFontWeight(QFont.Bold)

        # New format for drawing functions
        self.drawing_format = QTextCharFormat()
        self.drawing_format.setForeground(QColor("#00ff00"))  # Green
        self.drawing_format.setFontWeight(QFont.Bold)

        # New format for control keywords
        self.control_format = QTextCharFormat()
        self.control_format.setForeground(QColor("#42a5f5"))  # Light blue
        self.control_format.setFontWeight(QFont.Bold)

        # Format for syntax errors - Make it more visible
        self.error_format = QTextCharFormat()
        self.error_format.setUnderlineStyle(QTextCharFormat.WaveUnderline)
        self.error_format.setUnderlineColor(QColor(Qt.red))
        self.error_format.setForeground(QColor(Qt.red))  # Red text

        # New format for 'field' and 'instant' in violet
        self.violet_format = QTextCharFormat()
        self.violet_format.setForeground(QColor("#8A2BE2"))  # Violet
        self.violet_format.setFontWeight(QFont.Bold)

        # New format for 'set window size' and 'set window color' in sky blue
        self.light_blue_format = QTextCharFormat()
        self.light_blue_format.setForeground(QColor("#ADD8E6"))  # Sky blue
        self.light_blue_format.setFontWeight(QFont.Bold)

        # Format for 'set' in dark blue
        self.dark_blue_format = QTextCharFormat()
        self.dark_blue_format.setForeground(QColor("#0075f9"))  # Dark blue
        self.dark_blue_format.setFontWeight(QFont.Bold)

        # Format for 'true' and 'false' in orange
        self.orange_format = QTextCharFormat()
        self.orange_format.setForeground(QColor("#FFA500"))  # Orange
        self.orange_format.setFontWeight(QFont.Bold)

        # Formats for colors
        self.color_formats = {
            "red": QTextCharFormat(),
            "blue": QTextCharFormat(),
            "green": QTextCharFormat(),
            "yellow": QTextCharFormat(),
            "purple": QTextCharFormat(),
            "orange": QTextCharFormat(),
            "black": QTextCharFormat(),
            "white": QTextCharFormat(),
            "gray": QTextCharFormat(),
            "pink": QTextCharFormat(),
            "brown": QTextCharFormat(),
            "cyan": QTextCharFormat(),
            "magenta": QTextCharFormat()
        }

        # Define colors for each format
        for color, format in self.color_formats.items():
            format.setForeground(QColor(color))

        # Add rules
        # Add comment rules with proper regex patterns
        comment_regex = QRegExp("#.*$")  # Match # and everything after it until end of line
        comment_regex.setMinimal(True)
        self.rules.append((comment_regex, self.comment_format))
        
        self.add_rules(["var","func", "return","int","float","char"], self.keyword_format)
        self.add_rules(["draw circle", "draw line", "draw square", "draw rectangle", "draw triangle", "draw polygon", "draw ellipse", "draw arc"], self.drawing_format)
        self.add_rules(["do", "for", "while", "if", "else", "elif", "or", "and"], self.control_format)
        self.add_rules(["filled", "instant", "empty", "animated"], self.violet_format)
        self.add_rules(["set", "color", "size"], self.dark_blue_format)
        self.add_rules(["window", "cursor"], self.light_blue_format)
        self.add_rules(["true", "false"], self.orange_format)

        # Add rules for colors
        for color in self.color_formats.keys():
            self.add_rules([color], self.color_formats[color])


    def add_rules(self, patterns, text_format):
        """Adds highlighting rules."""
        for pattern in patterns:
            # Use regular expressions with \b for whole words
            regex = QRegExp(rf"\b{pattern}\b")
            self.rules.append((regex, text_format))

    def set_error_lines(self, error_lines):
        """Updates the lines containing syntax errors."""
        self.error_lines = error_lines
        # Clear tooltips for blocks that are no longer errors
        for block_number in range(self.document().blockCount()):
            block = self.document().findBlockByNumber(block_number)
            if block_number not in error_lines:
                block.setUserData(None)
        self.rehighlight()

    def highlightBlock(self, text):
        """Applies highlighting rules to the given text block."""
        block_number = self.currentBlock().blockNumber()

        # Handle multi-line comments first
        self.highlightMultilineComments(text)

        # Handle errors - Apply error format to the entire line first
        if block_number in self.error_lines:
            error_format = QTextCharFormat()
            error_format.setUnderlineStyle(QTextCharFormat.WaveUnderline)
            error_format.setUnderlineColor(QColor(Qt.red))
            self.setFormat(0, len(text), error_format)
            
            error_info = self.error_messages.get(block_number, {})
            error_msg = error_info.get('error', '')
            suggestion = error_info.get('suggestion', '')
            
            # Ensure we have valid strings for the tooltip
            error_msg = str(error_msg) if error_msg else "Unknown error"
            suggestion = str(suggestion) if suggestion else ""
            
            tooltip = f"""
            <div style='background-color: #2E2E2E; padding: 8px; border-radius: 4px; font-family: Arial;'>
                <div style='color: #FF0000; margin-bottom: 4px; text-align: center;'>⚠ Error ⚠</div>
                <div style='color: #FF0000; font-size: 12px; text-align: center;'>{error_msg}</div>
            """
            
            if suggestion:
                tooltip += f"<div style='color: #0066cc; font-size: 12px; text-align: center; margin-top: 4px;'>{suggestion}</div>"
            
            tooltip += "</div>"
            
            self.currentBlock().setUserData(TextBlockData(tooltip.strip()))

        # Apply syntax highlighting rules on top of error formatting
        for regex, text_format in self.rules:
            index = regex.indexIn(text)
            while index >= 0:
                length = regex.matchedLength()
                if block_number in self.error_lines:
                    # Create a combined format that preserves both syntax highlighting and error underlining
                    combined_format = QTextCharFormat(text_format)
                    combined_format.setUnderlineStyle(QTextCharFormat.WaveUnderline)
                    combined_format.setUnderlineColor(QColor(Qt.red))
                    self.setFormat(index, length, combined_format)
                else:
                    self.setFormat(index, length, text_format)
                index = regex.indexIn(text, index + length)

    def highlightMultilineComments(self, text):
        """Handle multi-line comment highlighting."""
        block = self.currentBlock()
        
        # If we're continuing a comment from previous block
        if block.previous().userState() == 1:
            # Format the entire block as a comment until we find an end marker
            endIndex = self.commentEndExpression.indexIn(text)
            if endIndex == -1:
                # No end found, entire block is a comment
                self.setFormat(0, len(text), self.multiLineCommentFormat)
                block.setUserState(1)
                return
            else:
                # Found end of comment
                length = endIndex + self.commentEndExpression.matchedLength()
                self.setFormat(0, length, self.multiLineCommentFormat)
                block.setUserState(0)
                # Continue processing rest of the line
                startIndex = length
        else:
            startIndex = 0
            block.setUserState(0)

        # Look for new comment starts in the rest of the text
        while startIndex >= 0:
            startIndex = self.commentStartExpression.indexIn(text, startIndex)
            if startIndex >= 0:
                endIndex = self.commentEndExpression.indexIn(text, startIndex)
                commentLength = 0
                if endIndex == -1:
                    # Comment continues to next line
                    commentLength = len(text) - startIndex
                    block.setUserState(1)
                else:
                    # Comment ends on this line
                    commentLength = endIndex - startIndex + self.commentEndExpression.matchedLength()
                    block.setUserState(0)
                
                self.setFormat(startIndex, commentLength, self.multiLineCommentFormat)
                
                if endIndex == -1:
                    break
                startIndex = endIndex + self.commentEndExpression.matchedLength()

        # If no comment start found and not in comment, reset state
        if startIndex == -1 and block.userState() != 1:
            block.setUserState(0)





####################### THIRD BIG PART ###########################""
class MyDrawppIDE(QMainWindow):
    '''this is main class of the ide, its from there that the instance of the application below is  created'''
    def __init__(self):
        super().__init__() #we herit from the QMainWindow class, which is basically the "big" widget from QWidget
        self.process = QProcess(self)
        
        # Store Draw++ location once at initialization
        current_dir = os.path.dirname(os.path.abspath(__file__))
        
        if os.path.basename(current_dir) == "Draw++":
            # Case 1: Running from ide.py in Draw++ folder
            self.draw_folder = current_dir
        elif "_MEI" in current_dir:
            # Case 2: Running from .exe (PyInstaller temp directory)
            self.draw_folder = current_dir
        else:
            # Case 3: Fallback
            raise Exception("Could not locate Draw++ folder. You are here: "+current_dir)

        # Set the window icon
        self.setWindowIcon(QIcon(os.path.join(self.draw_folder, "IDE", "Dpp_circle.ico")))
        # Set the history file path
        self.history_file = os.path.join(self.draw_folder, "IDE", ".history")
        # Initialize the UI
        self.init_ui()
        # Load the last opened file
        self.load_last_file()

    def init_ui(self):
        # configuration of the main window
        self.setGeometry(100, 100, 1000, 700) 
        #we can set the properties of the window, with basic CSS
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
        
        # creation for the tab widget, which is native to QWidget from PyQt5
        self.tab_widget = QTabWidget(self)
        self.setCentralWidget(self.tab_widget)
        self.tab_widget.setTabsClosable(True)  # Enable tab close buttons
        self.tab_widget.tabCloseRequested.connect(self.close_tab)  # Connect close signal

        #we can also set the properties of the window, with basic CSS again
        self.tab_widget.setStyleSheet(f"""
            QTabWidget::pane {{
                top: -1px;
                border-top: none;
            }}
            QTabBar::tab {{
                background: #2A2A2A;
                color: white;
                padding: 6px 12px;
                padding-right: 0px;
                padding-bottom: 10px;
                border: 1px solid #444444;
                border-bottom: none;
                margin-right: 2px;
                margin-bottom: -1px;
                min-width: 60px;
                max-width: 200px;
                font-size: 10px;
                border-top-left-radius: 6px;
                border-top-right-radius: 6px;
            }}
            QTabBar::tab:selected {{
                background: #1E1E1E;
                border-bottom: none;
                padding-bottom: 11px;
                font-size: 11px;
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

        # we set the toolbar, which is native to QWidget from PyQt5 again, we can design it with basic css
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

        # we set shortcuts, which are native to QWidget from PyQt5 again, we also connect button with their method, here open_file

        #in the next lines, its the same process, but for the others buttons
        open_action = QAction("Open", self)
        open_action.setShortcut("Ctrl+O")
        open_action.setToolTip("Open (Ctrl+O)")
        open_action.triggered.connect(self.open_file)
        open_action.triggered.connect(lambda: self.simulate_button_click(open_action))
        self.toolbar.addAction(open_action)

        # Séparator
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

        # Séparator again
        separator2 = QWidget()
        separator2.setFixedWidth(1)
        separator2.setStyleSheet("background-color: #1E1E1E;")
        self.toolbar.addWidget(separator2)

        save_as_action = QAction("Save As", self)
        save_as_action.setShortcut("Ctrl+Shift+S")
        save_as_action.setToolTip("Save As (Ctrl+Shift+S)")
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

        # Terminal toggle button, same process as the others
        terminal_action = QAction("Terminal", self)
        terminal_action.setShortcut("Ctrl+Shift+T")  # Common shortcut for terminal
        terminal_action.setToolTip("Terminal (Ctrl+Shift+T)")
        terminal_action.setCheckable(True)  # Make it checkable
        terminal_action.setChecked(False)  # Terminal is hidden by default
        terminal_action.triggered.connect(self.toggle_terminal)
        terminal_action.triggered.connect(lambda: self.simulate_button_click(terminal_action)) #this line is connecting a trigger button event to a process event
        self.toolbar.addAction(terminal_action)
        # Store reference to terminal action
        self.terminal_action = terminal_action

        # Séparateur
        separator8 = QWidget()
        separator8.setFixedWidth(1)
        separator8.setStyleSheet("background-color: #1E1E1E;")
        self.toolbar.addWidget(separator8)

        # Add search/replace shortcut
        search_action = QAction("Find/Replace", self)
        search_action.setShortcut("Ctrl+F")
        search_action.triggered.connect(self.show_search_dialog)
        search_action.setToolTip("Find/Replace (Ctrl+F)")
        search_action.triggered.connect(lambda: self.simulate_button_click(search_action))
        self.toolbar.addAction(search_action)

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
        """this method is called when the user clicks on the "New Tab" button in the toolbar. It creates a new tab and adds it to the tab widget"""

        #at the begining, we didnt had a hole open new tab class, we did the process into the init_ui method
        # but i realized by doing that way, there was only one execution of code possible cause there were only one instance for all tabs
        #so i created a new class for the open new tab method, and now each tab has its own instance, each one has his own process

        print("Création d'un nouvel onglet")

        # creation of a new tab and its properties
        new_tab = QWidget()
        layout = QVBoxLayout()
        layout.setContentsMargins(0, 0, 0, 0)  # Remove margins
        layout.setSpacing(0)  # Remove spacing between widgets

        # Create a splitter
        splitter = QSplitter(Qt.Vertical)
        splitter.setChildrenCollapsible(False)  # Prevent widgets from being collapsed to 0
        #we can set the properties of the splitter, with basic CSS
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

        # creation of the main editor, same as we done in the main window class
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

        # highlighter native method
        highlighter = SyntaxHighlighter(editor.document())

        # creation the terminal, same as we done in the main window class
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

        # Here I will position the button in the top right corner of the terminal
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

        # Store editor and terminal objects in a dictionary
        #it permit to each tabs to get his own editor, own terminal (so process connected to), and own highlighter
        if not hasattr(self, 'tabs_data'):
            self.tabs_data = {}
        self.tabs_data[new_tab] = {'editor': editor, 'terminal': terminal, 'highlighter': highlighter}

        # creating the new tab, just by adding in the list of tabs
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
        """this class update the window title when the tab change."""
        if index == -1:
            self.setWindowTitle("DrawStudioCode")  # Aucun onglet actif
        else:
            current_tab_title = self.tab_widget.tabText(index)
            self.setWindowTitle(f"DrawStudioCode - {current_tab_title}")
            # we used the actual index of the code and search into the list of tabs the name of the actual tab

    def open_file(self):
        """Open the file in a empty tab or automatically get to the opened file tab"""
        options = QFileDialog.Options()
        file_path, _ = QFileDialog.getOpenFileName(self, "Open File", "", "Draw++ Files (*.dpp);;All Files (*)", options=options)
        if file_path:
            # verify if the file is already opened
            file_name = os.path.basename(file_path)
            for i in range(self.tab_widget.count()):
                if self.tab_widget.tabText(i) == file_name:
                    # the file is already opened, we just need to switch to it
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
            
            with open(file_path, "r", newline='') as file:
                content = file.read()
                editor.setPlainText(content)
                self.tab_widget.setTabText(self.tab_widget.currentIndex(), file_name)
                editor.document().setModified(False)
                self.save_file_history(os.path.abspath(file_path))
            self.update_window_title(self.tab_widget.currentIndex())

    def save_file(self):
        """save the file in the current tab, if the tab name is "New tab", then we save it as a new file with the save_as_file method"""
        current_tab = self.tab_widget.currentWidget()
        if not current_tab:
            return

        current_file_name = self.tab_widget.tabText(self.tab_widget.currentIndex())
        if current_file_name.startswith('New Tab'):  #its a new file , we call save_as method
            self.save_as_file()  
        else:
            # Get editor from splitter's first widget
            splitter = current_tab.layout().itemAt(0).widget()
            editor = splitter.widget(0)  # First widget in splitter is the editor
            try:
                with open(current_file_name, "w") as file: #i'm opening the file in write mode, and copy/paste the text in the file
                    file.write(editor.toPlainText())
                editor.document().setModified(False)
                self.save_file_history(os.path.abspath(current_file_name))
            except Exception as e:
                QMessageBox.warning(self, "Save Error", f"Could not save file: {str(e)}")

    def save_as_file(self):
        """save the content of the active editor under a new name"""
        
        current_tab_name = self.tab_widget.tabText(self.tab_widget.currentIndex())
        # Get the name of the current tab
        if current_tab_name.startswith('New Tab'):
            current_tab_name = "MySuperDraw.dpp"
            # If the tab name starts with 'New Tab', change it to 'MySuperDraw.dpp'

        options = QFileDialog.Options()
        # Create options for the file dialog
        file_path, _ = QFileDialog.getSaveFileName(
            self, 
            "Save File", 
            current_tab_name,
            "Draw++ Files (*.dpp)", 
            options=options
        )
        # Open a file dialog to get the save file path
        if file_path:
            # Ensure file has .dpp extension
            if not file_path.endswith('.dpp'):
                file_path += '.dpp'
                
            current_tab = self.tab_widget.currentWidget()
            # Get the current tab widget
            splitter = current_tab.layout().itemAt(0).widget()
            # Get the splitter widget from the current tab's layout
            editor = splitter.widget(0)  # First widget in splitter is the editor
            try:
                with open(file_path, "w", newline='') as file:
                    file.write(editor.toPlainText())
                    # Write the editor's text to the file
                self.tab_widget.setTabText(self.tab_widget.currentIndex(), os.path.basename(file_path))
                # Update the tab text with the file name
                editor.document().setModified(False)
                # Mark the document as not modified
                self.save_file_history(os.path.abspath(file_path))
                # Save the file path to the file history
            except Exception as e:
                QMessageBox.warning(self, "Save Error", f"Could not save file: {str(e)}")
                # Show a warning message if the file could not be saved

    def execute_code(self, mode="run"):
        """execute the code of the active editor, depending on the mode specified (run/compile/debug)."""


        #go to the report to see how this method works, how i imagined the algorithm


        current_tab = self.tab_widget.currentWidget()
        # Get the current tab widget
        if not current_tab:
            return  # No active tab

        # Retrieve the editor and terminal associated with the active tab
        tab_data = self.tabs_data.get(current_tab)
        if not tab_data:
            return

        splitter = current_tab.layout().itemAt(0).widget()
        editor = splitter.widget(0)  # First widget in splitter is the editor
        terminal = tab_data['terminal']

        # Make sure terminal is visible and debug
        if mode == "debug" and not terminal.isVisible():
            self.toggle_terminal()

        # if we want to run the code, we need to save the file first, so we check it
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
                # Check if the user canceled the save
                if self.tab_widget.tabText(self.tab_widget.currentIndex()).startswith('New Tab'):
                    return  # User canceled the save, stop execution
            else:
                return  # User clicked Cancel, stop execution

        # Configure the command for the make file, not the same on windows actually
        if platform.system() == "Windows":
            make_command = "mingw32-make"
        else:
            make_command = "make"

        # Find the Draw++ folder
        draw_folder = self.draw_folder

        # Create the .to_compile.dpp file in the Draw++ folder, which is basically the file to compile after by PLY
        compile_file_path = os.path.join(draw_folder, ".to_compile.dpp")
        try:
            with open(compile_file_path, "w") as file:
                file.write(editor.toPlainText())
        except Exception as e:
            QMessageBox.critical(self, "Error", f"Could not write to {compile_file_path}: {str(e)}")
            return

        filename = "NAME=" + self.tab_widget.tabText(self.tab_widget.currentIndex())
        filename = filename.replace('.dpp', '')

        # Clear the terminal before executing
        terminal.clear()

        # Create a new process for this tab
        process = QProcess(self)
        if platform.system() != "Windows":
            # On Unix systems, make the process a group leader
            if hasattr(process, 'setProcessGroup'):
                process.setProcessGroup(QProcess.MergeProcessGroup)
        tab_data['process'] = process  # Store the process in the dictionary

        # Set the working directory to the Draw++ folder
        process.setWorkingDirectory(draw_folder)

        # Connect the outputs to the active terminal
        #a process can have 2 output, standard and error
        #the standard basically returns us the output in commande prompt of the process
        #here, this line can send all the output in the display output method
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

        # Configure the command and arguments according to the mode we chose
        #because the compile, and debug does the same command, but with different arguments, so we can use the same method
        command = make_command
        if mode == "compile":
            arguments = ["compile", filename]
        elif mode == "debug":
            arguments = ["DEBUG=1", filename]
        else:  # mode run
            arguments = [filename]

        # Start the process with the command and arguments
        process.start(command, arguments)

    def stop_process(self, tab):
        """Simply Stops the running process for the given tab, including any child processes."""
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
        """Called when a process finishes running, when an execution is killed by stop button, we reactive the run button"""
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
        """executes the code in debug mode : calls the execute_code method with the debug argument"""
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
        """display the output in the specified terminal with simple colors for certain sequences,"""
        #we get the readStandardOutput process return and we display it in the terminal
        
        #for windows, we rework the output by forcing the utf8 method for a readable output
        output = process.readAllStandardOutput().data().decode('utf-8', errors='ignore')

        #in the compilator, when an output starts by -#red, it means that the output is an error message, so we color it in red
        #same for -#green and -#blue
        
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
                # lines without -#red, -#green or -#blue are displayed in white
                self.append_colored_text(terminal, line + "\n", QColor("white"))

    def append_colored_text(self, terminal, text, color=QColor("white")):
        """add the text in the terminal with the specified color"""
        cursor = terminal.textCursor()
        cursor.movePosition(QTextCursor.End)  # Place le curseur à la fin
        text_format = QTextCharFormat()
        text_format.setForeground(color)  # Définit la couleur
        cursor.insertText(text, text_format)  # Insère le texte coloré
        terminal.setTextCursor(cursor)
        terminal.ensureCursorVisible()  # S'assure que le texte reste visible
        text_format.setForeground(QColor("white"))  # Définit la couleur

    def display_error(self, process, terminal):
        """Display the error in the termninal, these errors are not expected errors, they are errors that the program can't handle, like python error or gcc error : we use it in extreme debug case"""
        error_output = process.readAllStandardError().data().decode('utf-8', errors='ignore')

        for line in error_output.splitlines():  # Découpe en lignes
            self.append_colored_text(terminal, line + "\n", QColor("yellow"))  # Rouge par défaut pour les erreurs

    def show_new_window(self):
        """method connected to new window button, it creates a new window with a new tab, this window affect of self a new class of window
            which is here the AnotherWindow class, this window below is a copy of the  MyDrawppIDE class, but again , with its own comportment and process"""
        self.w = AnotherWindow()
        self.w.show()

    def close_tab(self, index):
        """close the tab at the specified index with a save request if necessary"""
        #the button close is affected to every tab, personnalize in the CSS, this is the method it calls
        #we get the actual tab index, and kill it, save it if it has been modified

        #getting the actual tab index
        tab = self.tab_widget.widget(index)
        if not tab: #if no tabs, go away dude !
            return

        # Get editor from splitter's first widget
        splitter = tab.layout().itemAt(0).widget()
        editor = splitter.widget(0)  # First widget in splitter is the editor
        
        if editor.document().isModified():  # if the document is modified
            reply = QMessageBox.question(
                self,
                "Save Changes",
                "Do you want to save changes before closing?",
                QMessageBox.Save | QMessageBox.Discard | QMessageBox.Cancel
            )

            if reply == QMessageBox.Save:
                # if the use wants to save
                current_file_name = self.tab_widget.tabText(index)
                if current_file_name.startswith('New Tab'):
                    # its a new file, we use save as
                    self.tab_widget.setCurrentIndex(index)  # Switch to the tab being closed, so the next index in the tab list
                    self.save_as_file()
                else:
                    # its an existing file, we save it
                    try:
                        with open(current_file_name, "w") as file:
                            file.write(editor.toPlainText())
                    except Exception as e:
                        QMessageBox.warning(self, "Save Error", f"Could not save file: {str(e)}")
                        return  # Ne pas fermer l'onglet si la sauvegarde échoue

            elif reply == QMessageBox.Cancel:
                return  #do not close the tab, we cancel the close, so sad..

        # clean the references in the tab list/data
        if tab in self.tabs_data:
            del self.tabs_data[tab]
        
        # finally kills and close the tab
        self.tab_widget.removeTab(index)

    def closeEvent(self, event):
        """handle the close event of the application, we ask the user if he wants to save the files before closing"""

        #check all the unsaved files by looping the tabs
        unsaved_tabs = []
        for i in range(self.tab_widget.count()):
            tab = self.tab_widget.widget(i)
            splitter = tab.layout().itemAt(0).widget()
            editor = splitter.widget(0)  # First widget in splitter is the editor
            if editor.document().isModified():
                unsaved_tabs.append((i, self.tab_widget.tabText(i)))

        if unsaved_tabs:
            # if unsaved files/tabs, we ask the user if he wants to save them
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
                # save all the unsaved files
                for tab_index, _ in unsaved_tabs:
                    self.tab_widget.setCurrentIndex(tab_index)
                    if self.tab_widget.tabText(tab_index).startswith('New Tab'):
                        self.save_as_file()
                    else:
                        self.save_file()
                    # check if the save was canceledl
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
        """loads the last file opened if there is one"""
        #the last file is stored in the history file, we just need to read it and open it
        try:
            if os.path.exists(self.history_file):
                with open(self.history_file, "r") as f:
                    last_file = f.read().strip()
                    # Read the last opened file path from the history file
                    if last_file and os.path.exists(last_file) and not last_file.startswith('New Tab'):
                        try:
                            with open(last_file, "r") as file:
                                current_tab = self.tab_widget.currentWidget()
                                # Get the current tab widget
                                splitter = current_tab.layout().itemAt(0).widget()
                                # Get the splitter widget from the current tab's layout
                                editor = splitter.widget(0)  # First widget in splitter is the editor
                                # Get the editor widget from the splitter
                                editor.setPlainText(file.read())
                                # Set the editor's text to the content of the last opened file
                                self.tab_widget.setTabText(self.tab_widget.currentIndex(), os.path.basename(last_file))
                                # Update the tab text with the file name
                                editor.document().setModified(False)
                                # Mark the document as not modified
                                self.update_window_title(self.tab_widget.currentIndex())
                                # Update the window title with the current tab index
                        except Exception as e:
                            print(f"Error reading last file {last_file}: {e}")
                            # If we can't read the last file, just start with a new tab
                            pass
        except Exception as e:
            print(f"Error loading history file: {e}")
        # Print an error message if the history file can't be loaded

    def save_file_history(self, file_path):
        """Sauvegarde le chemin du fichier dans l'historique."""
        try:
            # Only save if it's a real file path and not a new tab with 'new tab' title
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
        """Toggles the visibility of the terminal in the current tab"""
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

    def show_search_dialog(self):
        """Show the search and replace dialog."""
        current_tab = self.tab_widget.currentWidget()
        if not current_tab:
            return

        # Get the editor from the current tab
        splitter = current_tab.layout().itemAt(0).widget()
        editor = splitter.widget(0)

        # Create search dialog
        dialog = SearchDialog(editor, self)
        
        # Calculate position: align with right edge of window, below toolbar
        toolbar_height = self.toolbar.height()
        dialog_x = self.x() + self.width() - dialog.width() - 10  # 10px margin from right
        dialog_y = self.y() + toolbar_height + 50  # Increased from 5 to 35px margin below toolbar
        dialog.move(dialog_x, dialog_y)
        
        dialog.show()





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
    """ this class is used to create a new window, it is used to create a new window when the user clicks on the "New Window" button in the main window. """
    def __init__(self):
        super().__init__()




class SearchDialog(QWidget):
    def __init__(self, editor, parent=None):
        super().__init__(parent, Qt.Window)
        self.editor = editor
        self.setWindowTitle("Find/Replace")
        self.setWindowFlags(Qt.Window | Qt.WindowStaysOnTopHint)
        
        # Create layout
        layout = QVBoxLayout()
        layout.setSpacing(5)  # Reduce spacing between elements
        layout.setContentsMargins(5, 5, 5, 5)  # Reduce margins
        
        # Create search box
        search_layout = QHBoxLayout()
        search_layout.setSpacing(5)
        self.search_box = QLineEdit()
        self.search_box.setPlaceholderText("Find")
        self.search_box.returnPressed.connect(self.find_next)
        search_layout.addWidget(self.search_box)
        
        # Create replace box
        replace_layout = QHBoxLayout()
        replace_layout.setSpacing(5)
        self.replace_box = QLineEdit()
        self.replace_box.setPlaceholderText("Replace with")
        replace_layout.addWidget(self.replace_box)
        
        # Create buttons
        button_layout = QHBoxLayout()
        button_layout.setSpacing(3)  # Even smaller spacing for buttons
        
        self.case_sensitive = QCheckBox("Case Sens")
        button_layout.addWidget(self.case_sensitive)
        
        find_prev_btn = QPushButton("Prev")
        find_prev_btn.clicked.connect(self.find_previous)
        button_layout.addWidget(find_prev_btn)

        find_next_btn = QPushButton("Next")
        find_next_btn.clicked.connect(self.find_next)
        button_layout.addWidget(find_next_btn)
        
        replace_btn = QPushButton("Replace")
        replace_btn.clicked.connect(self.replace)
        button_layout.addWidget(replace_btn)
        
        replace_all_btn = QPushButton("All")
        replace_all_btn.clicked.connect(self.replace_all)
        button_layout.addWidget(replace_all_btn)
        
        # Add all layouts to main layout
        layout.addLayout(search_layout)
        layout.addLayout(replace_layout)
        layout.addLayout(button_layout)
        
        self.setLayout(layout)
        
        # Set fixed size
        self.setFixedSize(350, 120)
        
        # Style the dialog
        self.setStyleSheet("""
            QWidget {
                background-color: #2E2E2E;
                color: white;
                font-family: Consolas;
                font-size: 11px;
            }
            QLineEdit {
                background-color: #3E3E3E;
                border: 1px solid #555555;
                padding: 3px;
                selection-background-color: #666666;
                height: 20px;
            }
            QPushButton {
                background-color: #444444;
                border: none;
                padding: 3px 6px;
                min-width: 50px;
                height: 20px;
            }
            QPushButton:hover {
                background-color: #555555;
            }
            QPushButton:pressed {
                background-color: #666666;
            }
            QCheckBox {
                spacing: 3px;
            }
            QCheckBox::indicator {
                width: 12px;
                height: 12px;
            }
            QCheckBox::indicator:unchecked {
                background-color: #3E3E3E;
                border: 1px solid #555555;
            }
            QCheckBox::indicator:checked {
                background-color: #0075f9;
                border: 1px solid #555555;
            }
        """)
        
        # Set focus to search box
        self.search_box.setFocus()
        
        # If there's selected text, put it in the search box
        cursor = self.editor.textCursor()
        if cursor.hasSelection():
            self.search_box.setText(cursor.selectedText())
            self.search_box.selectAll()

    def find_next(self):
        """Find next occurrence of search text."""
        text = self.search_box.text()
        if not self.editor.find_text(text, True, self.case_sensitive.isChecked()):
            QMessageBox.information(self, "Find", "No more occurrences found.")

    def find_previous(self):
        """Find previous occurrence of search text."""
        text = self.search_box.text()
        if not self.editor.find_text(text, False, self.case_sensitive.isChecked()):
            QMessageBox.information(self, "Find", "No more occurrences found.")

    def replace(self):
        """Replace current selection if it matches find text."""
        find_text = self.search_box.text()
        replace_with = self.replace_box.text()
        if not self.editor.replace_text(find_text, replace_with, self.case_sensitive.isChecked()):
            self.find_next()

    def replace_all(self):
        """Replace all occurrences of find text."""
        find_text = self.search_box.text()
        replace_with = self.replace_box.text()
        count = self.editor.replace_all_text(find_text, replace_with, self.case_sensitive.isChecked())
        QMessageBox.information(self, "Replace All", f"Replaced {count} occurrence(s).")


############ EXECUCTION #############

# execution of the main guard
if __name__ == "__main__":

    app = QApplication(sys.argv)
    window = MyDrawppIDE()

    #these horrible lines are just to get our Logo in the little python executable while running on windows
    #only work on Windows
    if platform.system() == "Windows":
        icon = QIcon(os.path.join(window.draw_folder, "IDE", "Dpp_circle.ico"))
        app.setWindowIcon(icon)
        if hasattr(QtWin, 'setCurrentProcessExplicitAppUserModelID'):
            myappid = 'mon.application.ide.version1.0'
            QtWin.setCurrentProcessExplicitAppUserModelID(myappid)

    window.show()
    sys.exit(app.exec_())