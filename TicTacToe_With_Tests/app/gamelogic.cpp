#include "GameLogic.h"
#include "AIPlayer.h" // Now directly included due to INCLUDEPATH
#include "Board.h"    // Now directly included due to INCLUDEPATH
#include <QDebug>     // For debugging output

GameLogic::GameLogic(QObject *parent)
    : QObject(parent),
    aiPlayer(new AIPlayer(this)), // Initialize AIPlayer as a child of GameLogic
    currentPlayer(Board::PLAYER_X), // X always starts
    vsAI(false) // Initialize vsAI to false
{
    // Connect AIPlayer's move determined signal to GameLogic's slot
    connect(aiPlayer, &AIPlayer::moveDetermined, this, &GameLogic::onAiMoveDetermined);

    // This crucial connection tells GameLogic to tell AIPlayer to make a move
    connect(this, &GameLogic::aiMoveRequested, aiPlayer, &AIPlayer::makeMove);

    // REMOVED: These lines were incorrectly placed here (they belong in mainwindow.cpp)
    // connect(ui->easyRadioButton, &QRadioButton::toggled, [this](bool checked) { /* The difficulty is read when game starts */ });
    // connect(ui->hardRadioButton, &QRadioButton::toggled, [this](bool checked) { /* The difficulty is read when game starts */ });
}

void GameLogic::startGame(bool vsAI, const QString& aiDifficulty) {
    this->vsAI = vsAI; // Set the game mode (Vs AI or PvP)
    this->aiDifficulty = aiDifficulty; // Set AI difficulty if applicable
    resetGame(); // Reset the game state for a new game
}

void GameLogic::resetGame() {
    gameBoard.reset();        // Reset the underlying board
    currentPlayer = Board::PLAYER_X; // Reset current player to X
    moveHistory.clear();      // Clear move history
    emit currentPlayerChanged(currentPlayer); // Notify UI of player change
}

bool GameLogic::handlePlayerMove(int row, int col) {
    // Attempt to make the move on the board for the current player
    if (gameBoard.makeMove(row, col, currentPlayer)) {
        recordMove(row, col, currentPlayer);         // Record the move in history
        emit boardChanged(row, col, currentPlayer); // Notify UI about the board change (e.g., to display 'X' or 'O')

        int winner = gameBoard.checkWin(); // Check for a win after the move
        if (winner != Board::EMPTY) {
            processGameEnd(winner); // Game ended with a winner
            return true; // Game is over
        } else if (gameBoard.isFull()) {
            processGameEnd(Board::EMPTY); // Game ended in a draw (board is full, no winner)
            return true; // Game is over
        }

        switchPlayer(); // Switch to the next player's turn

        // If playing against AI AND it's currently AI's turn (Player O), request AI move
        if (vsAI && currentPlayer == Board::PLAYER_O) {
            // The `aiMoveRequested` signal will be connected to `AIPlayer::makeMove`
            // AIPlayer will then calculate its move and emit `moveDetermined`
            emit aiMoveRequested(gameBoard, aiDifficulty);
        }
        return true; // Move was valid and processed
    }
    return false; // Move was invalid (e.g., cell already occupied or out of bounds)
}

void GameLogic::switchPlayer() {
    // Toggle between PLAYER_X and PLAYER_O
    currentPlayer = (currentPlayer == Board::PLAYER_X) ? Board::PLAYER_O : Board::PLAYER_X;
    emit currentPlayerChanged(currentPlayer); // Notify UI of current player change (e.g., for status label)
}

void GameLogic::processGameEnd(int winner) {
    QString winnerString;
    if (winner == Board::PLAYER_X) {
        winnerString = "Player X wins!";
    } else if (winner == Board::PLAYER_O) {
        winnerString = vsAI ? "AI wins!" : "Player O wins!"; // Differentiate AI win from human Player O win
    } else { // winner == Board::EMPTY implies a draw when checked after game conclusion
        winnerString = "Draw";
    }
    emit gameEnded(winnerString, moveHistory); // Notify UI that the game has ended (for message box, history saving)
}

void GameLogic::recordMove(int row, int col, int player) {
    // Append the move to history in format "row:col:playerChar"
    moveHistory.append(QString("%1:%2:%3").arg(row).arg(col).arg((player == Board::PLAYER_X) ? 'X' : 'O'));
}

int GameLogic::getCurrentPlayer() const {
    return currentPlayer;
}

QStringList GameLogic::getMoveHistory() const {
    return moveHistory;
}

// Added getter for vsAI flag, crucial for MainWindow to differentiate PvP vs AI
bool GameLogic::isVsAI() const {
    return vsAI;
}

// This method returns the game outcome (winner or draw) or -2 if game is in progress
int GameLogic::getWinner() const {
    int winner = gameBoard.checkWin();
    if (winner != Board::EMPTY) {
        return winner; // Returns Board::PLAYER_X or Board::PLAYER_O
    } else if (gameBoard.isFull()) {
        return Board::EMPTY; // Indicates a draw (board full, no winner)
    }
    return -2; // Custom value: -2 signifies game is still in progress
}

void GameLogic::onAiMoveDetermined(const QPoint& move) {
    // This slot is called when the AIPlayer has calculated its move
    // Apply the AI's determined move to the board, using the current player (which should be AI's player)
    if (gameBoard.makeMove(move.x(), move.y(), currentPlayer)) {
        recordMove(move.x(), move.y(), currentPlayer);         // Record AI's move
        emit boardChanged(move.x(), move.y(), currentPlayer); // Notify UI about AI's move

        int winner = gameBoard.checkWin(); // Check for win after AI's move
        if (winner != Board::EMPTY) {
            processGameEnd(winner); // Game ended with AI winning
        } else if (gameBoard.isFull()) {
            processGameEnd(Board::EMPTY); // Game ended in a draw
        } else {
            switchPlayer(); // Switch back to player X (human's turn)
        }
    }
}
