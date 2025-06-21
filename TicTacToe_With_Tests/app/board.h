#ifndef BOARD_H
#define BOARD_H

#include <QPoint>
#include <vector> // Using std::vector for the board internal representation

class Board {
public:
    // Constants for player values (more descriptive than 1, -1)
    static const int EMPTY ;
    static const int PLAYER_X ;
    static const int PLAYER_O ;

    Board();
    void reset();
    bool makeMove(int row, int col, int player);
    int getCell(int row, int col) const;
    bool isFull() const;
    int checkWin() const; // Returns PLAYER_X, PLAYER_O, or EMPTY for no win/draw

    // For AI calculations (provides a copy of the internal state)
    std::vector<std::vector<int>> getBoardState() const;

private:
    std::vector<std::vector<int>> boardState; // The 3x3 game board
};

#endif // BOARD_H
