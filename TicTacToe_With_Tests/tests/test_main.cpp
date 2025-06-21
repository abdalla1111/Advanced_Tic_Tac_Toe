QT += testlib widgets sql
CONFIG += console testcase
CONFIG -= app_bundle
TEMPLATE = app

APP_DIR = ../app

INCLUDEPATH += $$APP_DIR \
               $$APP_DIR/core \
               $$APP_DIR/logic \
               $$APP_DIR/database \
               $$APP_DIR/utils

SOURCES += \
    test_main.cpp \
    tst_aiplayer.cpp \
    tst_databasemanager.cpp \
    tst_testboard.cpp \
    tst_gamelogic.cpp

SOURCES += \
    $$APP_DIR/board.cpp \
    $$APP_DIR/gamelogic.cpp \
    $$APP_DIR/AIPlayer.cpp \
    $$APP_DIR/DatabaseManager.cpp \
    $$APP_DIR/messagebox.cpp

HEADERS += \
    tst_aiplayer.h \
    tst_databasemanager.h \
    tst_testboard.h \
    tst_gamelogic.h
