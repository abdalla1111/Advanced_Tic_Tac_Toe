#ifndef TST_AIPLAYER_H
#define TST_AIPLAYER_H

#include <QtTest>
#include "aiplayer.h" // <-- Add this line back

class TestAIPlayer : public QObject
{
    Q_OBJECT

private slots:
    void testAiShouldWinWhenPossible();
    void testAiShouldBlockPlayerWin();
};

#endif // TST_AIPLAYER_H
