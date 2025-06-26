#include "messagebox.h"

namespace Utils {
void showStyledMessageBox(QWidget *parent, const QString &title, const QString &text, bool isWarning) {
    QMessageBox msgBox(parent);
    msgBox.setWindowTitle(title);
    msgBox.setText(text);
    msgBox.setIcon(isWarning ? QMessageBox::Warning : QMessageBox::Information);

    // The hardcoded stylesheet that was here has been REMOVED.
    // This allows the main window's stylesheet to apply correctly.

    msgBox.exec();
}
}
