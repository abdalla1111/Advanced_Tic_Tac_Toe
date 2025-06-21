#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <QMessageBox> // Needed for QMessageBox
#include <QWidget>     // Needed for QWidget* parent
#include <QString>     // Needed for QString

namespace Utils {
// A utility function to show styled message boxes consistently
void showStyledMessageBox(QWidget *parent, const QString &title, const QString &text, bool isWarning = false);
}

#endif // MESSAGEBOX_H
