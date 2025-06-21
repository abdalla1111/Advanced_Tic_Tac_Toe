TEMPLATE = app
CONFIG += console testcase
CONFIG -= app_bundle

QT += testlib widgets sql

# Use the robust top_srcdir variable to find the app directory
APP_DIR = $$top_srcdir/TicTacToe_With_Tests/app

# Add all necessary include paths for the compiler
INCLUDEPATH += $$APP_DIR \
               $$APP_DIR/core \
               $$APP_DIR/logic \
               $$APP_DIR/database \
               $$APP_DIR/utils

# List the test source files
SOURCES += \
    main.cpp \
    tst_aiplayer.cpp \
    tst_databasemanager.cpp \
    tst_testboard.cpp \
    tst_gamelogic.cpp

# Add the application's source files to be compiled with the tests
SOURCES += \
    $$APP_DIR/board.cpp \
    $$APP_DIR/gamelogic.cpp \
    $$APP_DIR/AIPlayer.cpp \
    $$APP_DIR/DatabaseManager.cpp \
    $$APP_DIR/messagebox.cpp

# List the test header files
HEADERS += \
    tst_aiplayer.h \
    tst_databasemanager.h \
    tst_testboard.h \
    tst_gamelogic.h
