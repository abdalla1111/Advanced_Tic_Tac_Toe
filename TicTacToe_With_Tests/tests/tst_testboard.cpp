#include "tst_testboard.h" // This line is crucial
#include "Board.h"

void TestBoard::initTestCase()
{
    // Will be called before the first test function is executed.
}

void TestBoard::cleanupTestCase()
{
    // Will be called after the last test function is executed.
}

void TestBoard::init()
{
    // Will be called before each test function is executed.
}

void TestBoard::cleanup()
{
    // Will be called after each test function is executed.
}

void TestBoard::testInitialBoardState()
{
    Board board;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            QCOMPARE(board.getBoardState()[i][j], Board::EMPTY);
        }
    }
}

void TestBoard::testMakeMoveValid()
{
    Board board;
    QVERIFY(board.makeMove(0, 0, Board::PLAYER_X));
    QCOMPARE(board.getBoardState()[0][0], Board::PLAYER_X);
}

void TestBoard::testMakeMoveInvalid()
{
    Board board;
    board.makeMove(0, 0, Board::PLAYER_X);
    QVERIFY(!board.makeMove(0, 0, Board::PLAYER_O)); // Already occupied
    QVERIFY(!board.makeMove(3, 0, Board::PLAYER_X)); // Out of bounds
}

void TestBoard::testCheckWinRows()
{
    Board board;
    board.makeMove(0, 0, Board::PLAYER_X);
    board.makeMove(0, 1, Board::PLAYER_X);
    board.makeMove(0, 2, Board::PLAYER_X);
    QCOMPARE(board.checkWin(), Board::PLAYER_X);

    board.reset();
    board.makeMove(1, 0, Board::PLAYER_O);
    board.makeMove(1, 1, Board::PLAYER_O);
    board.makeMove(1, 2, Board::PLAYER_O);
    QCOMPARE(board.checkWin(), Board::PLAYER_O);
}

void TestBoard::testCheckWinColumns()
{
    Board board;
    board.makeMove(0, 0, Board::PLAYER_X);
    board.makeMove(1, 0, Board::PLAYER_X);
    board.makeMove(2, 0, Board::PLAYER_X);
    QCOMPARE(board.checkWin(), Board::PLAYER_X);

    board.reset();
    board.makeMove(0, 1, Board::PLAYER_O);
    board.makeMove(1, 1, Board::PLAYER_O);
    board.makeMove(2, 1, Board::PLAYER_O);
    QCOMPARE(board.checkWin(), Board::PLAYER_O);
}

void TestBoard::testCheckWinDiagonals()
{
    Board board;
    board.makeMove(0, 0, Board::PLAYER_X);
    board.makeMove(1, 1, Board::PLAYER_X);
    board.makeMove(2, 2, Board::PLAYER_X);
    QCOMPARE(board.checkWin(), Board::PLAYER_X);

    board.reset();
    board.makeMove(0, 2, Board::PLAYER_O);
    board.makeMove(1, 1, Board::PLAYER_O);
    board.makeMove(2, 0, Board::PLAYER_O);
    QCOMPARE(board.checkWin(), Board::PLAYER_O);
}

void TestBoard::testCheckWinNoWin()
{
    Board board;
    board.makeMove(0, 0, Board::PLAYER_X);
    board.makeMove(0, 1, Board::PLAYER_O);
    board.makeMove(0, 2, Board::PLAYER_X);
    QCOMPARE(board.checkWin(), Board::EMPTY);
}

void TestBoard::testIsFull()
{
    Board board;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            board.makeMove(i, j, (i + j) % 2 == 0 ? Board::PLAYER_X : Board::PLAYER_O);
        }
    }
    QVERIFY(board.isFull());

    board.reset();
    QVERIFY(!board.isFull());
}

void TestBoard::testResetBoard()
{
    Board board;
    board.makeMove(0, 0, Board::PLAYER_X);
    board.reset();
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            QCOMPARE(board.getBoardState()[i][j], Board::EMPTY);
        }
    }
}
