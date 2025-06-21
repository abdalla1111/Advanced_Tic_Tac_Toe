#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include "Board.h"
#include <QObject>
#include <QPoint>
#include <QStringList>

class AIPlayer;

class GameLogic : public QObject {
    Q_OBJECT

public:
    explicit GameLogic(QObject *parent = nullptr);
    void startGame(bool vsAI, const QString& aiDifficulty);
    bool handlePlayerMove(int row, int col);
    void resetGame();
    int getCurrentPlayer() const;
    QStringList getMoveHistory() const;
    int getWinner() const;
    bool isVsAI() const; // Add this getter

signals:
    void boardChanged(int row, int col, int player);
    void gameEnded(const QString& winner, const QStringList& moves);
    void currentPlayerChanged(int player);
    void aiMoveRequested(const Board& currentBoard, const QString& difficulty);

private slots:
    void onAiMoveDetermined(const QPoint& move);

private:
    Board gameBoard;
    AIPlayer *aiPlayer;
    int currentPlayer;
    bool vsAI; // This is the flag we need to access
    QString aiDifficulty;
    QStringList moveHistory;

    void switchPlayer();
    void processGameEnd(int winner);
    void recordMove(int row, int col, int player);
};

#endif // GAMELOGIC_H
