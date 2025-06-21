
#include "Board.h"
const int Board::EMPTY = 0;
const int Board::PLAYER_X = 1;
const int Board::PLAYER_O = -1;

Board::Board() {
    reset(); // Initialize the board when constructed
}

void Board::reset() {
    // Reset all cells to EMPTY
    boardState.assign(3, std::vector<int>(3, EMPTY));
}

bool Board::makeMove(int row, int col, int player) {
    // Check for valid move: within bounds and cell is empty
    if (row >= 0 && row < 3 && col >= 0 && col < 3 && boardState[row][col] == EMPTY) {
        boardState[row][col] = player; // Place the player's mark
        return true; // Move successful
    }
    return false; // Invalid move
}

int Board::getCell(int row, int col) const {
    // Return the value of a cell if within bounds
    if (row >= 0 && row < 3 && col >= 0 && col < 3) {
        return boardState[row][col];
    }
    return EMPTY; // Return EMPTY for out-of-bounds access
}

bool Board::isFull() const {
    // Check if all cells are occupied
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (boardState[i][j] == EMPTY) {
                return false; // Found an empty cell, so board is not full
            }
        }
    }
    return true; // All cells are occupied
}

int Board::checkWin() const {
    // Check rows for a win
    for (int r = 0; r < 3; ++r) {
        if (boardState[r][0] != EMPTY &&
            boardState[r][0] == boardState[r][1] &&
            boardState[r][1] == boardState[r][2]) {
            return boardState[r][0]; // Return the winning player
        }
    }

    // Check columns for a win
    for (int c = 0; c < 3; ++c) {
        if (boardState[0][c] != EMPTY &&
            boardState[0][c] == boardState[1][c] &&
            boardState[1][c] == boardState[2][c]) {
            return boardState[0][c]; // Return the winning player
        }
    }

    // Check main diagonal for a win
    if (boardState[0][0] != EMPTY &&
        boardState[0][0] == boardState[1][1] &&
        boardState[1][1] == boardState[2][2]) {
        return boardState[0][0]; // Return the winning player
    }
    // Check anti-diagonal for a win
    if (boardState[0][2] != EMPTY &&
        boardState[0][2] == boardState[1][1] &&
        boardState[1][1] == boardState[2][0]) {
        return boardState[0][2]; // Return the winning player
    }

    return EMPTY; // No winner yet
}

std::vector<std::vector<int>> Board::getBoardState() const {
    return boardState; // Return a copy of the current board state
}
