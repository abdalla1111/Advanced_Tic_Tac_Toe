#ifndef TST_GAMELOGIC_H
#define TST_GAMELOGIC_H

#include <QObject>
#include <QtTest/QtTest>

class TestGameLogic : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testStartGame();
    void testHandlePlayerMove();
    void testResetGame();
    void testGetCurrentPlayer();
    void testGetWinner();
    void testIsVsAI();

};

#endif // TST_GAMELOGIC_H
