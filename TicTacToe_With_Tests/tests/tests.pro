// 1-4
QT += testlib widgets sql
CONFIG += console testcase
CONFIG -= app_bundle
TEMPLATE = app

// 5-8
APP_DIR = ../app
INCLUDEPATH += $$APP_DIR \
               $$APP_DIR/core \
               $$APP_DIR/logic \
               $$APP_DIR/database \
               $$APP_DIR/utils

// 9-16
SOURCES += \
    main.cpp \
    tst_aiplayer.cpp \
    tst_databasemanager.cpp \
    tst_testboard.cpp \
    tst_gamelogic.cpp

// 17-23
SOURCES += \
    $$APP_DIR/board.cpp \
    $$APP_DIR/gamelogic.cpp \
    $$APP_DIR/AIPlayer.cpp \
    $$APP_DIR/DatabaseManager.cpp \
    $$APP_DIR/messagebox.cpp

// 24-29
HEADERS += \
    tst_aiplayer.h \
    tst_databasemanager.h \
    tst_testboard.h \
    tst_gamelogic.h
