QT += testlib widgets sql
CONFIG += console testcase
CONFIG -= app_bundle
TEMPLATE = app

# Define the relative path to the app directory
APP_DIR = ../app

# Add the app's subdirectories to the include path for the compiler
INCLUDEPATH += $$APP_DIR \
               $$APP_DIR/core \
               $$APP_DIR/logic \
               $$APP_DIR/database \
               $$APP_DIR/utils

# List the source files for the test suite, using the new main file name
SOURCES += \
    test_main.cpp \
    tst_aiplayer.cpp \
    tst_databasemanager.cpp \
    tst_testboard.cpp \
    tst_gamelogic.cpp

# Add the application's source files to be linked with the tests
SOURCES += \
    $$APP_DIR/board.cpp \
    $$APP_DIR/gamelogic.cpp \
    $$APP_DIR/AIPlayer.cpp \
    $$APP_DIR/DatabaseManager.cpp \
    $$APP_DIR/messagebox.cpp

# List the test header files that require MOC
HEADERS += \
    tst_aiplayer.h \
    tst_databasemanager.h \
    tst_testboard.h \
    tst_gamelogic.h
    tst_databasemanager.h \
    tst_testboard.h \
    tst_gamelogic.h
