#include "AIPlayer.h"
#include <QRandomGenerator> // For random moves in easy difficulty
#include <QDebug>            // For debugging output
#include <algorithm>         // For std::max and std::min

AIPlayer::AIPlayer(QObject *parent) : QObject(parent), aiMoveDelayTimer(new QTimer(this)) {
    aiMoveDelayTimer->setSingleShot(true); // Ensures timer fires only once
    // Connect the timer's timeout signal to a lambda that determines and emits the AI's move
    connect(aiMoveDelayTimer, &QTimer::timeout, this, [this]() {
        QPoint move;
        if (currentDifficulty == "easy") {
            move = findRandomMove(currentAiBoard.getBoardState());
        } else { // Hard difficulty (minimax)
            move = findBestMove(currentAiBoard.getBoardState());
        }
        emit moveDetermined(move); // Emit the determined move
    });
}

void AIPlayer::makeMove(const Board& currentBoard, const QString& difficulty) {
    currentAiBoard = currentBoard; // Store a copy of the current board state for AI to work on
    currentDifficulty = difficulty;
    aiMoveDelayTimer->start(500); // Start the timer to simulate AI "thinking" time (500ms)
}

int AIPlayer::evaluateBoard(std::vector<std::vector<int>> board) {
    // Check rows for win
    for (int r = 0; r < 3; ++r) {
        if (board[r][0] == board[r][1] && board[r][1] == board[r][2] && board[r][0] != Board::EMPTY) {
            // AI (PLAYER_O) winning gets +10, Player (PLAYER_X) winning gets -10
            return (board[r][0] == Board::PLAYER_O) ? 10 : -10;
        }
    }

    // Check columns for win
    for (int c = 0; c < 3; ++c) {
        if (board[0][c] == board[1][c] && board[1][c] == board[2][c] && board[0][c] != Board::EMPTY) {
            return (board[0][c] == Board::PLAYER_O) ? 10 : -10;
        }
    }

    // Check diagonals for win
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != Board::EMPTY) {
        return (board[0][0] == Board::PLAYER_O) ? 10 : -10;
    }
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != Board::EMPTY) {
        return (board[0][2] == Board::PLAYER_O) ? 10 : -10;
    }

    return 0; // No winner
}

bool AIPlayer::isMovesLeft(std::vector<std::vector<int>> board) {
    // Check if there are any empty cells left on the board
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (board[i][j] == Board::EMPTY) {
                return true; // Found an empty cell
            }
        }
    }
    return false; // No moves left
}

// MODIFIED: Added depth, alpha, and beta parameters for Alpha-Beta Pruning
int AIPlayer::minimax(std::vector<std::vector<int>> board, int depth, bool isMaximizingPlayer, int alpha, int beta) {
    int score = evaluateBoard(board);

    // Terminal states (base cases for recursion: win, loss, or draw)
    if (score == 10) return score;  // AI (Maximizer) wins
    if (score == -10) return score; // Player (Minimizer) wins
    if (!isMovesLeft(board)) return 0; // It's a tie (draw)

    // Optional: add a depth limit if searching a very deep tree.
    // For Tic-Tac-Toe, the tree is small enough that a full search is fine.
    // if (depth >= MAX_SEARCH_DEPTH) return evaluateBoard(board); // Example: return heuristic at max depth

    // If this is maximizer's (AI's) move
    if (isMaximizingPlayer) {
        int best = -1000; // Initialize best value to a very small number

        // Traverse all cells
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                // Check if cell is empty
                if (board[i][j] == Board::EMPTY) {
                    board[i][j] = Board::PLAYER_O; // Make the move for AI
                    best = std::max(best, minimax(board, depth + 1, false, alpha, beta)); // Recur for opponent's turn
                    board[i][j] = Board::EMPTY; // Undo the move (backtrack)

                    alpha = std::max(alpha, best); // Update alpha value
                    if (beta <= alpha) { // Alpha-Beta Pruning condition:
                        // If current maximizer's best score (alpha) is already
                        // better than or equal to minimizer's guaranteed max (beta),
                        // minimizer will never pick this path. So, prune.
                        break; // Cut off this branch (inner loop)
                    }
                }
            }
            if (beta <= alpha) { // Check again after inner loop to break outer loop if needed
                break;
            }
        }
        return best;
    }
    // If this is minimizer's (Player's) move
    else {
        int best = 1000; // Initialize best value to a very large number

        // Traverse all cells
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                // Check if cell is empty
                if (board[i][j] == Board::EMPTY) {
                    board[i][j] = Board::PLAYER_X; // Make the move for Player
                    best = std::min(best, minimax(board, depth + 1, true, alpha, beta)); // Recur for AI's turn
                    board[i][j] = Board::EMPTY; // Undo the move

                    beta = std::min(beta, best); // Update beta value
                    if (beta <= alpha) { // Alpha-Beta Pruning condition:
                        // If current minimizer's best score (beta) is already
                        // worse than or equal to maximizer's guaranteed min (alpha),
                        // maximizer will never pick this path. So, prune.
                        break; // Cut off this branch (inner loop)
                    }
                }
            }
            if (beta <= alpha) { // Check again after inner loop to break outer loop if needed
                break;
            }
        }
        return best;
    }
}

// MODIFIED: Updated call to minimax to include new parameters
QPoint AIPlayer::findBestMove(std::vector<std::vector<int>> board) {
    // 1. Check if AI can win in the next move (priority 1) - no change needed here,
    //    as these are direct checks, not part of the recursive minimax search.
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == Board::EMPTY) {
                board[i][j] = Board::PLAYER_O;
                if (evaluateBoard(board) == 10) {
                    board[i][j] = Board::EMPTY; // Undo
                    return QPoint(i, j);
                }
                board[i][j] = Board::EMPTY; // Undo
            }
        }
    }
    // 2. Check if the player could win in the next move, and block them (priority 2) - no change needed here.
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == Board::EMPTY) {
                board[i][j] = Board::PLAYER_X;
                if (evaluateBoard(board) == -10) {
                    board[i][j] = Board::EMPTY; // Undo
                    return QPoint(i, j);
                }
                board[i][j] = Board::EMPTY; // Undo
            }
        }
    }
    // 3. If no immediate win/block, use minimax for optimal move (priority 3)
    int bestVal = -1000;
    QPoint bestMove(-1, -1);

    // Initial alpha and beta values for the top-level call
    int alpha = -1000; // Maximizer's best score found so far (initially negative infinity)
    int beta = 1000;   // Minimizer's best score found so far (initially positive infinity)

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (board[i][j] == Board::EMPTY) {
                board[i][j] = Board::PLAYER_O; // Make the move for AI

                // Call minimax with initial depth (0), and current alpha/beta values.
                // 'false' because after AI's move, it's the opponent's (minimizing) turn.
                int moveVal = minimax(board, 0, false, alpha, beta);

                board[i][j] = Board::EMPTY; // Undo the move

                if (moveVal > bestVal) {
                    bestMove.setX(i);
                    bestMove.setY(j);
                    bestVal = moveVal;
                }
                // Update alpha for the top-level move choice. This 'alpha' is for the current AI's perspective
                // on its best possible outcome from this top-level node.
                alpha = std::max(alpha, bestVal); // Update the alpha value for the main loop
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
        return QPoint(-1, -1); // Should not happen if game logic checks for full board before AI move
    }
    // Return a random available move
    return availableMoves[QRandomGenerator::global()->bounded(availableMoves.size())];
}
