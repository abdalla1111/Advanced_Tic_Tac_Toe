// File: app/aiplayer.h

#ifndef AIPLAYER_H
#define AIPLAYER_H

#include <QObject>
#include <QPoint>
#include <QTimer>
#include <vector>
#include "board.h"

// Forward-declare the test class before using it.
class TestAIPlayer;

class AIPlayer : public QObject
{
    Q_OBJECT

    // Add this line. It tells AIPlayer to allow TestAIPlayer
    // to access its private members.
    friend class TestAIPlayer;

public:
    explicit AIPlayer(QObject *parent = nullptr);

public slots:
    void makeMove(const Board& currentBoard, const QString& difficulty);

signals:
    void moveDetermined(const QPoint& move);

private:
    // Your test now has access to these functions
    QPoint findBestMove(std::vector<std::vector<int>> board);
    QPoint findMediumMove(std::vector<std::vector<int>> board); // NEW: Medium difficulty move finder
    QPoint findRandomMove(std::vector<std::vector<int>> board);

    int minimax(std::vector<std::vector<int>> board, int depth, bool isMaximizingPlayer, int alpha, int beta);
    int evaluateBoard(std::vector<std::vector<int>> board);
    bool isMovesLeft(std::vector<std::vector<int>> board);

    QTimer *aiMoveDelayTimer;
    Board currentAiBoard;
    QString currentDifficulty;
};

#endif // AIPLAYER_H
