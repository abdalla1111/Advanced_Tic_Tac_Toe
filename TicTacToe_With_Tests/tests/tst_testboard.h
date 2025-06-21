#ifndef TST_TESTBOARD_H
#define TST_TESTBOARD_H

#include <QObject>
#include <QtTest/QtTest>

// This is the full class definition. It must be in the .h file.
class TestBoard : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testInitialBoardState();
    void testMakeMoveValid();
    void testMakeMoveInvalid();
    void testCheckWinRows();
    void testCheckWinColumns();
    void testCheckWinDiagonals();
    void testCheckWinNoWin();
    void testIsFull();
    void testResetBoard();
};

#endif // TST_TESTBOARD_H
