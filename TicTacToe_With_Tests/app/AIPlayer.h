// File: app/aiplayer.h

#ifndef AIPLAYER_H
#define AIPLAYER_H

#include <QObject>
#include <QPoint>
#include <QTimer>
#include <vector>
#include "board.h"

// 1. Forward-declare the test class before using it.
class TestAIPlayer;

class AIPlayer : public QObject
{
    Q_OBJECT

    // 2. Add this line. It tells AIPlayer to allow TestAIPlayer
    //    to access its private members.
    friend class TestAIPlayer;

public:
    explicit AIPlayer(QObject *parent = nullptr);
    // ... other public members

public slots:
    void makeMove(const Board& currentBoard, const QString& difficulty);
    // ... other public slots

signals:
    void moveDetermined(const QPoint& move);

private:
    // Your test now has access to this function
    QPoint findBestMove(std::vector<std::vector<int>> board);

    // MODIFIED: Added depth, alpha, and beta parameters for Alpha-Beta Pruning
    int minimax(std::vector<std::vector<int>> board, int depth, bool isMaximizingPlayer, int alpha, int beta);
    QPoint findRandomMove(std::vector<std::vector<int>> board);
    int evaluateBoard(std::vector<std::vector<int>> board);
    bool isMovesLeft(std::vector<std::vector<int>> board);

    QTimer *aiMoveDelayTimer;
    Board currentAiBoard;
    QString currentDifficulty;
};

#endif // AIPLAYER_H
