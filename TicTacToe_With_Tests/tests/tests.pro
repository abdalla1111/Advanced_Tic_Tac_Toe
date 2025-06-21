QT += testlib widgets sql
CONFIG += console testcase
CONFIG -= app_bundle
TEMPLATE = app

# Point to the app project file to create a dependency
# This tells qmake that the 'tests' project depends on the 'app' project
SUBDIRS = ../app
subdirs.depends = ../app

# Add the app's source directory and subdirectories to the include path
INCLUDEPATH += ../app \
               ../app/core \
               ../app/logic \
               ../app/database \
               ../app/utils

# List the test source files
SOURCES += \
    main.cpp \
    tst_aiplayer.cpp \
    tst_databasemanager.cpp \
    tst_testboard.cpp \
    tst_gamelogic.cpp

# Add the application's source files so they are compiled and linked with the tests
SOURCES += \
    ../app/board.cpp \
    ../app/gamelogic.cpp \
    ../app/AIPlayer.cpp \
    ../app/DatabaseManager.cpp \
    ../app/messagebox.cpp

# List the test header files that require MOC
HEADERS += \
    tst_aiplayer.h \
    tst_databasemanager.h \
    tst_testboard.h \
    tst_gamelogic.h
