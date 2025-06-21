# This file manages the sub-projects.

TEMPLATE = subdirs

# List the subdirectories that contain the other .pro files.
# qmake will automatically find app.pro in the 'app' folder and tests.pro in the 'tests' folder.
SUBDIRS = \
    app \
    tests

# This is a crucial line. It tells Qt to always build the 'app'
# project before it builds the 'tests' project, because the tests
# depend on the code from the app.
tests.depends = app
