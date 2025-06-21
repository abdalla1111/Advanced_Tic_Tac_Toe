QT += testlib widgets sql
CONFIG += console testcase
CONFIG -= app_bundle
TEMPLATE = app

# Define the relative path to the app directory.
# From the 'tests' folder, this is one level up and into the 'app' folder.
APP_DIR = ../app

# Add the app directory AND its subdirectories to the include path.
# This tells the compiler where to find headers like "board.h" and "AIPlayer.h".
INCLUDEPATH += $$APP_DIR \
               $$APP_DIR/core \
               $$APP_DIR/logic \
               $$APP_DIR/database \
               $$APP_DIR/utils

# List the source files FOR THE TEST SUITE ITSELF.
SOURCES += \
    test_main.cpp \
    tst_aiplayer.cpp \
    tst_databasemanager.cpp \
    tst_testboard.cpp \
    tst_gamelogic.cpp

# Also, list THE APPLICATION'S source files.
# They need to be compiled and linked with the tests to create the final test executable.
SOURCES += \
    $$APP_DIR/board.cpp \
    $$APP_DIR/gamelogic.cpp \
    $$APP_DIR/AIPlayer.cpp \
    $$APP_DIR/DatabaseManager.cpp \
    $$APP_DIR/messagebox.cpp

# List the test header files that use Q_OBJECT and need the moc.
HEADERS += \
    tst_aiplayer.h \
    tst_databasemanager.h \
    tst_testboard.h \
    tst_gamelogic.h
