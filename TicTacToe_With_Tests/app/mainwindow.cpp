#include "mainwindow.h"
#include "ui_mainwindow.h" // Essential: This header defines Ui::MainWindow

// Include custom classes
#include "DatabaseManager.h"
#include "GameLogic.h"
#include "Board.h" // For Board::PLAYER_X, Board::PLAYER_O constants
#include "MessageBox.h"

// Standard Qt headers
#include <QMessageBox>
#include <QRandomGenerator>
#include <QDateTime>
#include <QDebug>
#include <QCheckBox>
#include <QPushButton>
#include <QListWidgetItem>
#include <QSettings> // Required for persistent session management
#include <QTimer>    // Use QTimer for delayed, safe UI updates


MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::MainWindow),
    replayTimer(new QTimer(this)),
    replayIndex(0),
    isMessageBoxActive(false),
    m_loginInProgress(false),
    m_isReplayMode(false) // *FIX: Initialize replay mode flag*
{
    ui->setupUi(this);

    dbManager = new DatabaseManager(this);
    gameLogic = new GameLogic(this);

    setupConnections();

    ui->hardRadioButton->setChecked(true); // Default to hard AI difficulty

    // --- Persistent Session Management: Check for auto-login ---
    QSettings settings("YourCompanyName", "TicTacToe");
    QString savedUsername = settings.value("lastLoggedInUser").toString();

    if (!savedUsername.isEmpty()) {
        currentUser = savedUsername;
        ui->stackedWidget->setCurrentWidget(ui->page_1_main);

        QString welcomeMessage = "Welcome back, " + savedUsername + "!";

        QTimer::singleShot(100, this, [this, welcomeMessage]() {
            if (isMessageBoxActive) return;
            isMessageBoxActive = true;
            Utils::showStyledMessageBox(this, "Auto-Login Successful", welcomeMessage);
            isMessageBoxActive = false;
        });
    } else {
        ui->stackedWidget->setCurrentWidget(ui->page_0_login);
    }
    // --- End Persistent Session Management ---
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupConnections()
{
    // Connections are correct, no changes needed here.
    // --- Login Page Connections ---
    connect(ui->loginButton, &QPushButton::clicked, this, &MainWindow::on_loginButton_clicked);
    connect(ui->signupLoginPageButton, &QPushButton::clicked, this, &MainWindow::on_signupLoginPageButton_clicked);
    connect(ui->showLoginPasswordCheckBox, &QCheckBox::toggled, this, &MainWindow::on_showLoginPasswordCheckBox_toggled);
    connect(ui->noWorriesButton, &QPushButton::clicked, this, &MainWindow::on_noWorriesButton_clicked);

    // --- Sign-Up Page Connections ---
    connect(ui->registerButton, &QPushButton::clicked, this, &MainWindow::on_registerButton_clicked);
    connect(ui->showSignupPasswordCheckBox, &QCheckBox::toggled, this, &MainWindow::on_showSignupPasswordCheckBox_toggled);
    connect(ui->loginButtonSignupPage, &QPushButton::clicked, this, &MainWindow::on_loginButtonSignupPage_clicked);

    // --- Main Page Connections ---
    connect(ui->playerVsAiButton, &QPushButton::clicked, this, &MainWindow::on_playerVsAiButton_clicked);
    connect(ui->playerVsPlayerButton, &QPushButton::clicked, this, &MainWindow::on_playerVsPlayerButton_clicked);
    connect(ui->myAccountButton, &QPushButton::clicked, this, &MainWindow::on_myAccountButton_clicked);
    connect(ui->myGameHistoryButton, &QPushButton::clicked, this, &MainWindow::on_myGameHistoryButton_clicked);

    // --- Password Reset Page Connections ---
    connect(ui->resetPasswordButton, &QPushButton::clicked, this, &MainWindow::on_resetPasswordButton_clicked);
    connect(ui->showResetNewPasswordCheckBox, &QCheckBox::toggled, this, &MainWindow::on_showResetNewPasswordCheckBox_toggled);
    connect(ui->backButtonReset, &QPushButton::clicked, this, &MainWindow::on_backButtonReset_clicked);

    // --- Personal Info Page Connections ---
    connect(ui->backButtonAccount, &QPushButton::clicked, this, &MainWindow::on_backButtonAccount_clicked);
    connect(ui->changePasswordButton, &QPushButton::clicked, this, &MainWindow::on_changePasswordButton_clicked);
    connect(ui->logoutButtonAccount, &QPushButton::clicked, this, &MainWindow::on_logoutButtonAccount_clicked);

    // --- Game Board Page Connections ---
    connect(ui->pushButton_00, &QPushButton::clicked, this, &MainWindow::handleBoardClick);
    connect(ui->pushButton_01, &QPushButton::clicked, this, &MainWindow::handleBoardClick);
    connect(ui->pushButton_02, &QPushButton::clicked, this, &MainWindow::handleBoardClick);
    connect(ui->pushButton_10, &QPushButton::clicked, this, &MainWindow::handleBoardClick);
    connect(ui->pushButton_11, &QPushButton::clicked, this, &MainWindow::handleBoardClick);
    connect(ui->pushButton_12, &QPushButton::clicked, this, &MainWindow::handleBoardClick);
    connect(ui->pushButton_20, &QPushButton::clicked, this, &MainWindow::handleBoardClick);
    connect(ui->pushButton_21, &QPushButton::clicked, this, &MainWindow::handleBoardClick);
    connect(ui->pushButton_22, &QPushButton::clicked, this, &MainWindow::handleBoardClick);
    connect(ui->resetGameboardButton, &QPushButton::clicked, this, &MainWindow::on_resetGameboardButton_clicked);
    connect(ui->backButtonGamePage, &QPushButton::clicked, this, &MainWindow::on_backButtonGamePage_clicked);

    // --- Game History Page Connections ---
    connect(ui->replayGameButton, &QPushButton::clicked, this, &MainWindow::on_replayGameButton_clicked);
    connect(ui->deleteGameButton, &QPushButton::clicked, this, &MainWindow::on_deleteGameButton_clicked);
    connect(ui->backButtonHistory, &QPushButton::clicked, this, &MainWindow::on_backButtonHistory_clicked);

    // --- Replay Timer Connection ---
    connect(replayTimer, &QTimer::timeout, this, &MainWindow::replayNextMove);

    // Connect GameLogic signals to MainWindow slots
    connect(gameLogic, &GameLogic::boardChanged, this, &MainWindow::onBoardChanged);
    connect(gameLogic, &GameLogic::gameEnded, this, &MainWindow::onGameEnded);
    connect(gameLogic, &GameLogic::currentPlayerChanged, this, &MainWindow::onCurrentPlayerChanged);
}

void MainWindow::on_loginButton_clicked() {
    if (m_loginInProgress) {
        return;
    }
    m_loginInProgress = true;
    ui->loginButton->setEnabled(false);

    QString username = ui->loginUsernameLineEdit->text();
    QString password = ui->loginPasswordLineEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        Utils::showStyledMessageBox(this, "Login Failed", "Please enter both username and password.", true);
        ui->loginButton->setEnabled(true);
        m_loginInProgress = false;
        return;
    }

    if (dbManager->authenticateUser(username, password)) {
        currentUser = username;
        ui->stackedWidget->setCurrentWidget(ui->page_1_main);
        ui->loginUsernameLineEdit->clear();
        ui->loginPasswordLineEdit->clear();

        QString welcomeMessage = "Welcome, " + username + "!";

        QTimer::singleShot(100, this, [this, welcomeMessage]() {
            if (isMessageBoxActive) return;
            isMessageBoxActive = true;
            Utils::showStyledMessageBox(this, "Login Successful", welcomeMessage);
            isMessageBoxActive = false;
        });

        QSettings settings("YourCompanyName", "TicTacToe");
        settings.setValue("lastLoggedInUser", username);

    } else {
        Utils::showStyledMessageBox(this, "Login Failed", "Invalid username or password.", true);
        ui->loginUsernameLineEdit->clear();
        ui->loginPasswordLineEdit->clear();
        ui->loginButton->setEnabled(true);
        m_loginInProgress = false;
    }
}

void MainWindow::on_signupLoginPageButton_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->page_2_signup);
    ui->signupUsernameLineEdit->clear();
    ui->signupPasswordLineEdit->clear();
    ui->signupFirstNameLineEdit->clear();
    ui->signupLastNameLineEdit->clear();
    ui->showSignupPasswordCheckBox->setChecked(false);
}

void MainWindow::on_registerButton_clicked() {
    QString username = ui->signupUsernameLineEdit->text();
    QString password = ui->signupPasswordLineEdit->text();
    QString firstName = ui->signupFirstNameLineEdit->text();
    QString lastName = ui->signupLastNameLineEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        Utils::showStyledMessageBox(this, "Signup Failed", "Username and Password are required.", true);
        return;
    }

    if (dbManager->registerUser(username, password, firstName, lastName)) {
        ui->stackedWidget->setCurrentWidget(ui->page_0_login);
        ui->signupUsernameLineEdit->clear();
        ui->signupPasswordLineEdit->clear();
        ui->signupFirstNameLineEdit->clear();
        ui->signupLastNameLineEdit->clear();
        ui->showSignupPasswordCheckBox->setChecked(false);

        QTimer::singleShot(100, this, [this]() {
            if (isMessageBoxActive) return;
            isMessageBoxActive = true;
            Utils::showStyledMessageBox(this, "Signup Successful", "You have been registered.");
            isMessageBoxActive = false;
        });
    } else {
        Utils::showStyledMessageBox(this, "Signup Failed", "Could not register user. Username might already exist.", true);
        ui->signupUsernameLineEdit->clear();
        ui->signupPasswordLineEdit->clear();
        ui->signupFirstNameLineEdit->clear();
        ui->signupLastNameLineEdit->clear();
        ui->showSignupPasswordCheckBox->setChecked(false);
    }
}

void MainWindow::on_loginButtonSignupPage_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_0_login);
}

void MainWindow::on_noWorriesButton_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->page_3_password_reset);
    ui->resetUsernameLineEdit->clear();
    ui->resetNewPasswordLineEdit->clear();
    ui->resetConfirmPasswordLineEdit->clear();
    ui->showResetNewPasswordCheckBox->setChecked(false);
}

void MainWindow::on_resetPasswordButton_clicked() {
    QString username = ui->resetUsernameLineEdit->text();
    QString newPassword = ui->resetNewPasswordLineEdit->text();
    QString confirmPassword = ui->resetConfirmPasswordLineEdit->text();

    if (username.isEmpty() || newPassword.isEmpty() || confirmPassword.isEmpty()) {
        Utils::showStyledMessageBox(this, "Reset Failed", "Please fill in all required fields.", true);
        return;
    }
    if (newPassword != confirmPassword) {
        Utils::showStyledMessageBox(this, "Reset Failed", "Passwords do not match.", true);
        return;
    }

    if (dbManager->resetUserPassword(username, newPassword)) {
        ui->stackedWidget->setCurrentWidget(ui->page_0_login);
        ui->resetUsernameLineEdit->clear();
        ui->resetNewPasswordLineEdit->clear();
        ui->resetConfirmPasswordLineEdit->clear();
        ui->showResetNewPasswordCheckBox->setChecked(false);

        QTimer::singleShot(100, this, [this]() {
            if (isMessageBoxActive) return;
            isMessageBoxActive = true;
            Utils::showStyledMessageBox(this, "Success", "Your password has been reset.");
            isMessageBoxActive = false;
        });
    } else {
        Utils::showStyledMessageBox(this, "Reset Failed", "Could not reset password for the given username.", true);
        ui->resetUsernameLineEdit->clear();
        ui->resetNewPasswordLineEdit->clear();
        ui->resetConfirmPasswordLineEdit->clear();
        ui->showResetNewPasswordCheckBox->setChecked(false);
    }
}

void MainWindow::togglePasswordVisibility(QLineEdit* lineEdit, bool visible) {
    lineEdit->setEchoMode(visible ? QLineEdit::Normal : QLineEdit::Password);
}

void MainWindow::on_showLoginPasswordCheckBox_toggled(bool checked) {
    togglePasswordVisibility(ui->loginPasswordLineEdit, checked);
}

void MainWindow::on_showSignupPasswordCheckBox_toggled(bool checked) {
    togglePasswordVisibility(ui->signupPasswordLineEdit, checked);
}

void MainWindow::on_showResetNewPasswordCheckBox_toggled(bool checked) {
    togglePasswordVisibility(ui->resetNewPasswordLineEdit, checked);
    togglePasswordVisibility(ui->resetConfirmPasswordLineEdit, checked);
}

void MainWindow::on_backButtonReset_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->page_0_login);
}

void MainWindow::on_backButtonAccount_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->page_1_main);
}

void MainWindow::on_changePasswordButton_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->page_3_password_reset);
}

void MainWindow::on_logoutButtonAccount_clicked() {
    currentUser.clear();
    QSettings settings("YourCompanyName", "TicTacToe");
    settings.remove("lastLoggedInUser");
    ui->stackedWidget->setCurrentWidget(ui->page_0_login);
    ui->loginButton->setEnabled(true);
    m_loginInProgress = false;
}

// *FIX: This function's logic is now updated to handle all replay reset cases.*
void MainWindow::on_resetGameboardButton_clicked() {
    // If we are in any kind of replay state (running or finished)
    if (m_isReplayMode) {
        replayTimer->stop(); // Ensure timer is stopped before restarting
        replayIndex = 0;
        resetBoardUI();      // Clears text and enables buttons for replay
        disableGameboardUI(); // Re-disable board for replay
        replayTimer->start(800); // Start the animation from the beginning
        ui->gameStatusLabel->setText("Replaying Game (Restarted)...");
    } else {
        // If not in replay mode, it's a live game. Reset it.
        gameLogic->resetGame();
        resetBoardUI();
        ui->gameStatusLabel->setText("Player X's Turn");
    }
}

// *FIX: Reset replay mode when going back.*
void MainWindow::on_backButtonGamePage_clicked() {
    if (replayTimer->isActive()) {
        replayTimer->stop();
        replayMoves.clear();
        replayIndex = 0;
    }
    m_isReplayMode = false; // We are no longer in replay mode
    ui->stackedWidget->setCurrentWidget(ui->page_1_main);
}

// *FIX: Set replay mode to false for live games.*
void MainWindow::on_playerVsAiButton_clicked() {
    m_isReplayMode = false;
    QString aiDifficulty = ui->easyRadioButton->isChecked() ? "easy" : "hard";
    gameLogic->startGame(true, aiDifficulty);
    resetBoardUI();
    ui->gameStatusLabel->setText("Player X's Turn");
    ui->stackedWidget->setCurrentWidget(ui->page_5_gameboard);
}

// *FIX: Set replay mode to false for live games.*
void MainWindow::on_playerVsPlayerButton_clicked() {
    m_isReplayMode = false;
    gameLogic->startGame(false, "");
    resetBoardUI();
    ui->gameStatusLabel->setText("Player X's Turn");
    ui->stackedWidget->setCurrentWidget(ui->page_5_gameboard);
}

void MainWindow::handleBoardClick() {
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    if (!clickedButton || !clickedButton->text().isEmpty()) {
        return;
    }

    // Disallow clicks during a replay
    if (m_isReplayMode) {
        Utils::showStyledMessageBox(this, "Replay Active", "Cannot make moves during a game replay. Please use Reset or Back.", true);
        return;
    }

    int row, col;
    QString name = clickedButton->objectName();
    row = name.at(name.length() - 2).digitValue();
    col = name.at(name.length() - 1).digitValue();

    gameLogic->handlePlayerMove(row, col);
}

void MainWindow::on_myAccountButton_clicked() {
    QVariantMap userInfo = dbManager->getUserInfo(currentUser);
    updateAccountInfoUI(userInfo);
    ui->stackedWidget->setCurrentWidget(ui->page_4_personal_info);
}

void MainWindow::updateAccountInfoUI(const QVariantMap& userInfo) {
    ui->accountFirstNameLabel->setText(userInfo["firstName"].toString());
    ui->accountLastNameLabel->setText(userInfo["lastName"].toString());
    ui->accountUsernameLabel->setText(userInfo["username"].toString());
}

void MainWindow::on_myGameHistoryButton_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->page_6_game_history);
    loadGameHistoryUI();
}

void MainWindow::loadGameHistoryUI() {
    ui->gameHistoryListWidget->clear();
    QList<QVariantMap> history = dbManager->loadGameHistory(currentUser);

    if (history.isEmpty()) {
        ui->gameHistoryListWidget->addItem("No game history available.");
        return;
    }

    for (const QVariantMap& item : history) {
        QString opponent = (item["player1"].toString() == currentUser) ? item["player2"].toString() : item["player1"].toString();
        QString entry = QString("vs %1 | Result: %2 | On: %3")
                            .arg(opponent)
                            .arg(item["result"].toString())
                            .arg(item["timestamp"].toDateTime().toString("yyyy-MM-dd hh:mm"));
        QListWidgetItem *listItem = new QListWidgetItem(entry, ui->gameHistoryListWidget);
        listItem->setData(Qt::UserRole, item["id"].toInt());
    }
}

void MainWindow::on_deleteGameButton_clicked() {
    QListWidgetItem *selectedItem = ui->gameHistoryListWidget->currentItem();
    if (selectedItem) {
        int gameId = selectedItem->data(Qt::UserRole).toInt();
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Confirm Delete", "Are you sure you want to delete this game history?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            if (dbManager->deleteGameHistory(gameId)) {
                delete selectedItem;
                Utils::showStyledMessageBox(this, "Success", "Game history deleted.");
                if (ui->gameHistoryListWidget->count() == 0) {
                    ui->gameHistoryListWidget->addItem("No game history available.");
                }
            } else {
                Utils::showStyledMessageBox(this, "Error", "Failed to delete game history.", true);
            }
        }
    } else {
        Utils::showStyledMessageBox(this, "Error", "Please select a game to delete.", true);
    }
}

void MainWindow::on_backButtonHistory_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->page_1_main);
}

void MainWindow::replayNextMove() {
    if (replayIndex < replayMoves.size()) {
        QStringList parts = replayMoves[replayIndex].split(":");
        int row = parts[0].toInt();
        int col = parts[1].toInt();
        QString playerChar = parts[2];
        QPushButton *button = getButton(row, col);
        if(button) {
            button->setText(playerChar);
            if(playerChar == 'X') button->setStyleSheet("color: #3498db;");
            else button->setStyleSheet("color: #e74c3c;");
        }
        replayIndex++;
    } else {
        replayTimer->stop();
        Utils::showStyledMessageBox(this, "Replay Finished", "The game replay has concluded.");
        disableGameboardUI(); // Disable board to prevent clicks on squares after replay
    }
}

// *FIX: Set replay mode to true when starting a replay.*
void MainWindow::on_replayGameButton_clicked() {
    QListWidgetItem *selectedItem = ui->gameHistoryListWidget->currentItem();
    if (selectedItem) {
        int gameId = selectedItem->data(Qt::UserRole).toInt();
        QString movesString = dbManager->getGameMoves(gameId);
        if (!movesString.isEmpty()) {
            m_isReplayMode = true; // We are now in replay mode
            replayMoves = movesString.split(",");
            resetBoardUI();
            disableGameboardUI();
            replayIndex = 0;
            replayTimer->start(800);
            ui->stackedWidget->setCurrentWidget(ui->page_5_gameboard);
            ui->gameStatusLabel->setText("Replaying Game...");
        } else {
            Utils::showStyledMessageBox(this, "Replay Game", "Failed to load game moves for replay.", true);
        }
    } else {
        Utils::showStyledMessageBox(this, "Replay Game", "Please select a game to replay.", true);
    }
}

void MainWindow::onBoardChanged(int row, int col, int player) {
    QPushButton* button = getButton(row, col);
    if (button) {
        button->setText((player == Board::PLAYER_X) ? "X" : "O");
        button->setStyleSheet((player == Board::PLAYER_X) ? "color: #3498db;" : "color: #e74c3c;");
        button->setEnabled(false);
    }
}

void MainWindow::onGameEnded(const QString& winner, const QStringList& moves) {
    if (!m_isReplayMode) { // Only show for live games
        Utils::showStyledMessageBox(this, "Game Over", winner);
        QString player2Name = gameLogic->isVsAI() ? "AI" : "Player O";
        dbManager->saveGameHistory(currentUser, player2Name, winner, moves);
    }
    disableGameboardUI();
}

void MainWindow::onCurrentPlayerChanged(int player) {
    if (!m_isReplayMode) { // Only update for live games
        if (player == Board::PLAYER_X) {
            ui->gameStatusLabel->setText("Player X's Turn");
            enableGameboardUI();
        } else {
            if (gameLogic->isVsAI()) {
                ui->gameStatusLabel->setText("AI's Turn");
                disableGameboardUI();
            } else {
                ui->gameStatusLabel->setText("Player O's Turn");
                enableGameboardUI();
            }
        }
    }
}

void MainWindow::resetBoardUI() {
    QList<QPushButton*> buttons = ui->groupBox_3->findChildren<QPushButton*>();
    for(QPushButton* button : buttons) {
        button->setText("");
        button->setStyleSheet("");
        button->setEnabled(true);
    }
}

QPushButton* MainWindow::getButton(int row, int col) {
    return ui->groupBox_3->findChild<QPushButton*>(QString("pushButton_%1%2").arg(row).arg(col));
}

void MainWindow::disableGameboardUI() {
    QList<QPushButton*> buttons = ui->groupBox_3->findChildren<QPushButton*>();
    for(auto button : buttons) {
        button->setEnabled(false);
    }
}

void MainWindow::enableGameboardUI() {
    // During a live game, only enable empty buttons
    if (!m_isReplayMode) {
        QList<QPushButton*> buttons = ui->groupBox_3->findChildren<QPushButton*>();
        for(auto button : buttons) {
            if(button->text().isEmpty()){
                button->setEnabled(true);
            } else {
                button->setEnabled(false);
            }
        }
    } else {
        // During a replay, all buttons should be disabled to prevent user clicks.
        disableGameboardUI();
    }
}

void MainWindow::updateGameboardUI() {
    // This method is not currently used but is kept for potential future use.
}
