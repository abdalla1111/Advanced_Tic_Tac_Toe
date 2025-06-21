QT += testlib widgets sql # <--- Ensure 'widgets' is present here
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app

APP_SRC_DIR = ../app
INCLUDEPATH += $$APP_SRC_DIR

SOURCES += \
    main.cpp \
    tst_aiplayer.cpp \
    tst_databasemanager.cpp \
    tst_testboard.cpp \
    tst_gamelogic.cpp \
    $$APP_SRC_DIR/board.cpp \
    $$APP_SRC_DIR/aiplayer.cpp \
    $$APP_SRC_DIR/gamelogic.cpp \
    $$APP_SRC_DIR/databasemanager.cpp

HEADERS += \
    tst_aiplayer.h \
    tst_databasemanager.h \
    tst_testboard.h \
    tst_gamelogic.h \
    $$APP_SRC_DIR/aiplayer.h \
    $$APP_SRC_DIR/gamelogic.h \
    $$APP_SRC_DIR/board.h \
    $$APP_SRC_DIR/databasemanager.h
