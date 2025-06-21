QT += core gui widgets sql
CONFIG += c++17

INCLUDEPATH += $$PWD/core \
               $$PWD/logic \
               $$PWD/database \
               $$PWD/utils

FORMS += mainwindow.ui

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    Board.cpp \
    GameLogic.cpp \
    AIPlayer.cpp \
    DatabaseManager.cpp \
    MessageBox.cpp


HEADERS += \
    mainwindow.h \
    Board.h \
    GameLogic.h \
    AIPlayer.h \
    DatabaseManager.h \
    MessageBox.h

RESOURCES += i18n.qrc
TRANSLATIONS += tic_tac_toe_3_en_US.ts
