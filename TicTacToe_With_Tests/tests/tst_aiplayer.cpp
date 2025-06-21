#include "tst_aiplayer.h"
#include "board.h" // You must include the header for the Board class

void TestAIPlayer::testAiShouldWinWhenPossible() {
    AIPlayer ai;
    Board board;

    board.makeMove(0, 0, Board::PLAYER_O);
    board.makeMove(1, 0, Board::PLAYER_X);
    board.makeMove(0, 1, Board::PLAYER_O);
    board.makeMove(1, 1, Board::PLAYER_X);

    QPoint bestMove = ai.findBestMove(board.getBoardState());
    QCOMPARE(bestMove, QPoint(0, 2));
}

void TestAIPlayer::testAiShouldBlockPlayerWin() {
    AIPlayer ai;
    Board board;

    board.makeMove(0, 0, Board::PLAYER_X);
    board.makeMove(0, 1, Board::PLAYER_O);
    board.makeMove(1, 1, Board::PLAYER_X);
    board.makeMove(0, 2, Board::PLAYER_O);

    QPoint bestMove = ai.findBestMove(board.getBoardState());
    QCOMPARE(bestMove, QPoint(2, 2));
}

#include "tst_aiplayer.moc"
