#!/usr/bin/env python3
import sys
import os

from PyQt5.QtWidgets import QApplication, QMainWindow, QTextEdit, QAction, QToolButton, QMessageBox
from PyQt5.QtGui import QIcon, QColor
from PyQt5.QtCore import QSize, Qt

# Try to import the compiled module.
# In a real installation, it would be in site-packages.
try:
    import QtRibbonWidget
except ImportError:
    print("Error: QtRibbonWidget module not found. Make sure it is built and in PYTHONPATH.")
    sys.exit(1)

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("QtRibbonWidget Python Demo")
        self.resize(1000, 700)

        # 1. Create the Ribbon
        self.ribbon = QtRibbonWidget.Ribbon(self)
        self.setMenuWidget(self.ribbon) # Use as menu bar

        # 2. Central widget
        self.text_edit = QTextEdit()
        self.setCentralWidget(self.text_edit)
        self.text_edit.setPlainText("This is a demo of the QtRibbonWidget Python bindings.\n\n"
                                    "The ribbon above is created using PyQt5 and the SIP-generated "
                                    "QtRibbonWidget module.")

        # 3. Add some content to the Ribbon
        self.init_ribbon()

        # Status bar
        self.statusBar().showMessage("Ready")

    def init_ribbon(self):
        # --- Home Tab ---
        self.ribbon.addTab(QIcon(":/icons/briefcase_1.svg"), "Home")

        # File Group
        new_act = QAction(QIcon(":/icons/create_new_2.svg"), "New", self)
        new_act.triggered.connect(lambda: self.log("New file created"))
        self.ribbon.addAction("Home", "File", new_act, QtRibbonWidget.RibbonButtonGroup.LargeButton)

        open_act = QAction(QIcon(":/icons/live_folder_2.svg"), "Open", self)
        open_act.triggered.connect(lambda: self.log("Open file dialog"))
        self.ribbon.addAction("Home", "File", open_act, QtRibbonWidget.RibbonButtonGroup.LargeButton)

        save_act = QAction(QIcon(":/icons/save_2.svg"), "Save", self)
        save_act.triggered.connect(lambda: self.log("Save file"))
        self.ribbon.addAction("Home", "File", save_act, QtRibbonWidget.RibbonButtonGroup.SmallButton)

        close_act = QAction(QIcon(":/icons/file_2.svg"), "Close", self)
        close_act.triggered.connect(self.close)
        self.ribbon.addAction("Home", "File", close_act, QtRibbonWidget.RibbonButtonGroup.SmallButton)

        # Edit Group
        copy_act = QAction(QIcon(":/icons/create_new_2.svg"), "Copy", self)
        copy_act.triggered.connect(self.text_edit.copy)
        self.ribbon.addAction("Home", "Edit", copy_act, QtRibbonWidget.RibbonButtonGroup.LargeButton)

        paste_act = QAction(QIcon(":/icons/live_folder_2.svg"), "Paste", self)
        paste_act.triggered.connect(self.text_edit.paste)
        self.ribbon.addAction("Home", "Edit", paste_act, QtRibbonWidget.RibbonButtonGroup.LargeButton)

        # --- View Tab ---
        self.ribbon.addTab(QIcon(":/icons/monitor_1.svg"), "View")

        # Theme Group
        themes = [
            ("Default", QtRibbonWidget.Ribbon.DefaultTheme),
            ("Office 2013", QtRibbonWidget.Ribbon.Office2013Theme),
            ("Office 2016 Blue", QtRibbonWidget.Ribbon.Office2016BlueTheme),
            ("Dark", QtRibbonWidget.Ribbon.DarkTheme)
        ]

        for name, theme_id in themes:
            act = QAction(name, self)
            act.triggered.connect(lambda checked, t=theme_id: self.ribbon.setTheme(t))
            self.ribbon.addAction("View", "Themes", act, QtRibbonWidget.RibbonButtonGroup.SmallButton)

        # Mode Group
        normal_act = QAction(QIcon(":/icons/monitor_1.svg"), "Normal Mode", self)
        normal_act.triggered.connect(lambda: self.ribbon.setRibbonMode(QtRibbonWidget.Ribbon.NormalMode))
        self.ribbon.addAction("View", "Display Mode", normal_act, QtRibbonWidget.RibbonButtonGroup.SmallButton)

        min_act = QAction(QIcon(":/icons/monitor_1.svg"), "Minimized Mode", self)
        min_act.triggered.connect(lambda: self.ribbon.setRibbonMode(QtRibbonWidget.Ribbon.MinimizedMode))
        self.ribbon.addAction("View", "Display Mode", min_act, QtRibbonWidget.RibbonButtonGroup.SmallButton)

        # --- Help Tab ---
        self.ribbon.addTab(QIcon(":/icons/information_1.svg"), "Help")
        about_act = QAction(QIcon(":/icons/information_1.svg"), "About", self)
        about_act.triggered.connect(lambda: QMessageBox.about(self, "About", "QtRibbonWidget Python Demo"))
        self.ribbon.addAction("Help", "Information", about_act, QtRibbonWidget.RibbonButtonGroup.LargeButton)

        # Connect the bubbled-up signal
        self.ribbon.actionTriggered.connect(self.on_action_triggered)

    def log(self, message):
        self.text_edit.append(message)
        self.statusBar().showMessage(message, 3000)

    def on_action_triggered(self, action):
        print(f"Ribbon action triggered: {action.text()}")

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
