#include "AIPlayer.h"
#include <QRandomGenerator>
#include <QDebug>
#include <algorithm>

AIPlayer::AIPlayer(QObject *parent) : QObject(parent), aiMoveDelayTimer(new QTimer(this)) {
    aiMoveDelayTimer->setSingleShot(true);
    connect(aiMoveDelayTimer, &QTimer::timeout, this, [this]() {
        QPoint move;
        if (currentDifficulty == "easy") {
            move = findRandomMove(currentAiBoard.getBoardState());
        } else if (currentDifficulty == "medium") { // NEW: Handle medium difficulty
            move = findMediumMove(currentAiBoard.getBoardState());
        } else { // Hard difficulty
            move = findBestMove(currentAiBoard.getBoardState());
        }
        emit moveDetermined(move);
    });
}

void AIPlayer::makeMove(const Board& currentBoard, const QString& difficulty) {
    currentAiBoard = currentBoard;
    currentDifficulty = difficulty;
    aiMoveDelayTimer->start(500);
}

// NEW: Implementation for the medium difficulty AI
QPoint AIPlayer::findMediumMove(std::vector<std::vector<int>> board) {
    // Priority 1: Check if AI can win in the next move
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == Board::EMPTY) {
                board[i][j] = Board::PLAYER_O; // Try AI move
                if (evaluateBoard(board) == 10) {
                    return QPoint(i, j); // Return winning move
                }
                board[i][j] = Board::EMPTY; // Undo move
            }
        }
    }

    // Priority 2: Check if the player could win, and block them
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == Board::EMPTY) {
                board[i][j] = Board::PLAYER_X; // Try Player move
                if (evaluateBoard(board) == -10) {
                    return QPoint(i, j); // Return blocking move
                }
                board[i][j] = Board::EMPTY; // Undo move
            }
        }
    }

    // Priority 3: If no immediate win/block, make a random move
    return findRandomMove(board);
}


int AIPlayer::evaluateBoard(std::vector<std::vector<int>> board) {
    // Check rows
    for (int r = 0; r < 3; ++r) {
        if (board[r][0] == board[r][1] && board[r][1] == board[r][2] && board[r][0] != Board::EMPTY) {
            return (board[r][0] == Board::PLAYER_O) ? 10 : -10;
        }
    }
    // Check columns
    for (int c = 0; c < 3; ++c) {
        if (board[0][c] == board[1][c] && board[1][c] == board[2][c] && board[0][c] != Board::EMPTY) {
            return (board[0][c] == Board::PLAYER_O) ? 10 : -10;
        }
    }
    // Check diagonals
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != Board::EMPTY) {
        return (board[0][0] == Board::PLAYER_O) ? 10 : -10;
    }
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != Board::EMPTY) {
        return (board[0][2] == Board::PLAYER_O) ? 10 : -10;
    }
    return 0; // No winner
}

bool AIPlayer::isMovesLeft(std::vector<std::vector<int>> board) {
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            if (board[i][j] == Board::EMPTY)
                return true;
    return false;
}

int AIPlayer::minimax(std::vector<std::vector<int>> board, int depth, bool isMaximizingPlayer, int alpha, int beta) {
    int score = evaluateBoard(board);
    if (score == 10) return score;
    if (score == -10) return score;
    if (!isMovesLeft(board)) return 0;

    if (isMaximizingPlayer) {
        int best = -1000;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (board[i][j] == Board::EMPTY) {
                    board[i][j] = Board::PLAYER_O;
                    best = std::max(best, minimax(board, depth + 1, false, alpha, beta));
                    board[i][j] = Board::EMPTY;
                    alpha = std::max(alpha, best);
                    if (beta <= alpha)
                        break;
                }
            }
            if (beta <= alpha)
                break;
        }
        return best;
    }
    else {
        int best = 1000;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (board[i][j] == Board::EMPTY) {
                    board[i][j] = Board::PLAYER_X;
                    best = std::min(best, minimax(board, depth + 1, true, alpha, beta));
                    board[i][j] = Board::EMPTY;
                    beta = std::min(beta, best);
                    if (beta <= alpha)
                        break;
                }
            }
            if (beta <= alpha)
                break;
        }
        return best;
    }
}

QPoint AIPlayer::findBestMove(std::vector<std::vector<int>> board) {
    // Priority 1 & 2 are handled by findMediumMove, so we can reuse it for the opening checks.
    // QPoint immediateMove = findMediumMove(board);
    if(evaluateBoard(board) != 0) {
        // If findMediumMove finds a winning or blocking move, it will not be random.
        // A random move is only returned if no win/block is found.
        // We can check this by seeing if making the move and evaluating it results in a win/loss.
        // This is a bit complex. A simpler way is to just run the checks here again.
        for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) {
                if (board[i][j] == Board::EMPTY) {
                    board[i][j] = Board::PLAYER_O;
                    if (evaluateBoard(board) == 10) { return QPoint(i, j); }
                    board[i][j] = Board::EMPTY;
                }
            }
        for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) {
                if (board[i][j] == Board::EMPTY) {
                    board[i][j] = Board::PLAYER_X;
                    if (evaluateBoard(board) == -10) { return QPoint(i, j); }
                    board[i][j] = Board::EMPTY;
                }
            }
    }


    // Priority 3: If no immediate win/block, use minimax for optimal move
    int bestVal = -1000;
    QPoint bestMove(-1, -1);
    int alpha = -1000;
    int beta = 1000;

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (board[i][j] == Board::EMPTY) {
                board[i][j] = Board::PLAYER_O;
                int moveVal = minimax(board, 0, false, alpha, beta);
                board[i][j] = Board::EMPTY;
                if (moveVal > bestVal) {
                    bestMove.setX(i);
                    bestMove.setY(j);
                    bestVal = moveVal;
                }
                alpha = std::max(alpha, bestVal);
            }
        }
    }
    return bestMove;
}

QPoint AIPlayer::findRandomMove(std::vector<std::vector<int>> board) {
    QVector<QPoint> availableMoves;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (board[i][j] == Board::EMPTY) {
                availableMoves.append(QPoint(i, j));
            }
        }
    }
    if (availableMoves.isEmpty()) {
        return QPoint(-1, -1);
    }
    return availableMoves[QRandomGenerator::global()->bounded(availableMoves.size())];
}
