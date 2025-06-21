#include "MessageBox.h"

namespace Utils {
void showStyledMessageBox(QWidget *parent, const QString &title, const QString &text, bool isWarning) {
    QMessageBox msgBox(parent); // Create message box with parent
    msgBox.setWindowTitle(title); // Set title
    msgBox.setText(text);         // Set text
    msgBox.setIcon(isWarning ? QMessageBox::Warning : QMessageBox::Information); // Set icon based on warning flag
    // Apply custom stylesheet for a modern dark theme
    msgBox.setStyleSheet(
        "QMessageBox {"
        "    background-color: #34495e;"
        "    color: #ecf0f1;"
        "    font-family: 'Segoe UI', sans-serif;"
        "}"
        "QLabel {"
        "    color: #ecf0f1;"
        "    font-size: 12pt;"
        "}"
        "QPushButton {"
        "    background-color: #1abc9c;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 8px;"
        "    padding: 10px;"
        "    min-width: 80px;"
        "    font: bold 10pt 'Segoe UI';"
        "}"
        "QPushButton:hover {"
        "    background-color: #16a085;"
        "}"
        );
    msgBox.exec(); // Display the message box
}
}
