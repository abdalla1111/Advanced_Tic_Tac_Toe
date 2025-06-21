#include "tst_gamelogic.h"
#include "GameLogic.h"
#include "Board.h"

void TestGameLogic::initTestCase()
{
    // Will be called before the first test function is executed.
}

void TestGameLogic::cleanupTestCase()
{
    // Will be called after the last test function is executed.
}

void TestGameLogic::init()
{
    // Will be called before each test function is executed.
}

void TestGameLogic::cleanup()
{
    // Will be called after each test function is executed.
}

void TestGameLogic::testStartGame()
{
    GameLogic logic;
    logic.startGame(false, ""); // Test PvP
    QCOMPARE(logic.getCurrentPlayer(), Board::PLAYER_X);
    QCOMPARE(logic.isVsAI(), false);

    logic.startGame(true, "Easy"); // Test Vs AI
    QCOMPARE(logic.getCurrentPlayer(), Board::PLAYER_X);
    QCOMPARE(logic.isVsAI(), true);
}

void TestGameLogic::testHandlePlayerMove()
{
    GameLogic logic;
    logic.startGame(false, "");

    // Test valid move
    QVERIFY(logic.handlePlayerMove(0, 0));
    QCOMPARE(logic.getCurrentPlayer(), Board::PLAYER_O);

    // Test invalid move (already occupied)
    QVERIFY(!logic.handlePlayerMove(0, 0));
    QCOMPARE(logic.getCurrentPlayer(), Board::PLAYER_O); // Player should not change

    // Test a win condition for Player X
    logic.resetGame();
    logic.handlePlayerMove(0, 0); // X
    logic.handlePlayerMove(1, 0); // O
    logic.handlePlayerMove(0, 1); // X
    logic.handlePlayerMove(1, 1); // O
    QVERIFY(logic.handlePlayerMove(0, 2)); // X wins
    QCOMPARE(logic.getWinner(), Board::PLAYER_X);

    // Test a draw condition (MODIFIED LOGIC FOR DRAW)
    logic.resetGame();
    logic.handlePlayerMove(0, 0); // X
    logic.handlePlayerMove(1, 1); // O
    logic.handlePlayerMove(0, 2); // X
    logic.handlePlayerMove(0, 1); // O
    logic.handlePlayerMove(2, 1); // X
    logic.handlePlayerMove(2, 0); // O
    logic.handlePlayerMove(1, 0); // X
    logic.handlePlayerMove(2, 2); // O
    QVERIFY(logic.handlePlayerMove(1, 2)); // X makes last move, results in draw
    QCOMPARE(logic.getWinner(), Board::EMPTY);
}

void TestGameLogic::testResetGame()
{
    GameLogic logic;
    logic.startGame(false, "");
    logic.handlePlayerMove(0, 0);
    logic.resetGame();
    QCOMPARE(logic.getCurrentPlayer(), Board::PLAYER_X);
    QCOMPARE(logic.getMoveHistory().count(), 0);
    // Add a check to ensure the board is actually reset (e.g., all cells are empty)
    // This would require a getter for the board state in GameLogic or Board class
}

void TestGameLogic::testGetCurrentPlayer()
{
    GameLogic logic;
    logic.startGame(false, "");
    QCOMPARE(logic.getCurrentPlayer(), Board::PLAYER_X);
    logic.handlePlayerMove(0, 0);
    QCOMPARE(logic.getCurrentPlayer(), Board::PLAYER_O);
}

void TestGameLogic::testGetWinner()
{
    GameLogic logic;
    logic.startGame(false, "");

    // No winner yet
    QCOMPARE(logic.getWinner(), -2);

    // Player X wins
    logic.handlePlayerMove(0, 0); // X
    logic.handlePlayerMove(1, 0); // O
    logic.handlePlayerMove(0, 1); // X
    logic.handlePlayerMove(1, 1); // O
    logic.handlePlayerMove(0, 2); // X wins
    QCOMPARE(logic.getWinner(), Board::PLAYER_X);

    // Draw (MODIFIED LOGIC FOR DRAW)
    logic.resetGame();
    logic.handlePlayerMove(0, 0); // X
    logic.handlePlayerMove(1, 1); // O
    logic.handlePlayerMove(0, 2); // X
    logic.handlePlayerMove(0, 1); // O
    logic.handlePlayerMove(2, 1); // X
    logic.handlePlayerMove(2, 0); // O
    logic.handlePlayerMove(1, 0); // X
    logic.handlePlayerMove(2, 2); // O
    logic.handlePlayerMove(1, 2); // X makes last move, results in draw
    QCOMPARE(logic.getWinner(), Board::EMPTY);
}

void TestGameLogic::testIsVsAI()
{
    GameLogic logic;
    logic.startGame(false, "");
    QCOMPARE(logic.isVsAI(), false);

    logic.startGame(true, "Hard");
    QCOMPARE(logic.isVsAI(), true);
}

#include "tst_gamelogic.moc"
