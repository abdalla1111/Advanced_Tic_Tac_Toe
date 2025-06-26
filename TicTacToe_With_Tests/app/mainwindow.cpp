#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "DatabaseManager.h"
#include "gamelogic.h"
#include "board.h"
#include "messagebox.h"

#include <QMessageBox>
#include <QRandomGenerator>
#include <QDateTime>
#include <QDebug>
#include <QPushButton>
#include <QListWidgetItem>
#include <QSettings>
#include <QTimer>
#include <QIcon>

// --- MODIFIED: CONSTRUCTOR NOW HANDLES AUTO-LOGIN ---
MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::MainWindow),
    replayTimer(new QTimer(this)),
    replayIndex(0),
    isMessageBoxActive(false),
    m_loginInProgress(false),
    m_isReplayMode(false)
{
    ui->setupUi(this);

    dbManager = new DatabaseManager(this);
    gameLogic = new GameLogic(this);

    setupConnections();

    // Check for a remembered user to auto-login
    QSettings settings("YourCompanyName", "TicTacToe");
    QString autoLoginUser = settings.value("rememberedIdentifier").toString();

    if (!autoLoginUser.isEmpty()) {
        // If a user is remembered, log them in automatically
        QVariantMap userInfo = dbManager->getUserInfo(autoLoginUser);
        currentUser = userInfo["username"].toString();

        ui->stackedWidget->setCurrentWidget(ui->page_1_main); // Go directly to the main page

        // Show a welcome message after a short delay
        QString welcomeMessage = "Welcome back, " + currentUser + "!";
        QTimer::singleShot(100, this, [this, welcomeMessage]() {
            if (isMessageBoxActive) return;
            isMessageBoxActive = true;
            Utils::showStyledMessageBox(this, "Login Successful", welcomeMessage);
            isMessageBoxActive = false;
        });

    } else {
        // Otherwise, show the login page as normal
        ui->stackedWidget->setCurrentWidget(ui->page_0_login);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupConnections()
{
    // --- Password Visibility Connections ---
    connect(ui->showLoginPasswordButton, &QPushButton::clicked, this, &MainWindow::handleShowPassword);
    connect(ui->showSignupPasswordButton, &QPushButton::clicked, this, &MainWindow::handleShowPassword);
    connect(ui->showResetPasswordButton, &QPushButton::clicked, this, &MainWindow::handleShowPassword);

    // --- Page Navigation and Actions ---
    connect(ui->loginButton, &QPushButton::clicked, this, &MainWindow::on_loginButton_clicked);
    connect(ui->signupLoginPageButton, &QPushButton::clicked, this, &MainWindow::on_signupLoginPageButton_clicked);
    connect(ui->noWorriesButton, &QPushButton::clicked, this, &MainWindow::on_noWorriesButton_clicked);
    connect(ui->registerButton, &QPushButton::clicked, this, &MainWindow::on_registerButton_clicked);
    connect(ui->loginButtonSignupPage, &QPushButton::clicked, this, &MainWindow::on_loginButtonSignupPage_clicked);
    connect(ui->playerVsAiButton, &QPushButton::clicked, this, &MainWindow::on_playerVsAiButton_clicked);
    connect(ui->playerVsPlayerButton, &QPushButton::clicked, this, &MainWindow::on_playerVsPlayerButton_clicked);
    connect(ui->myAccountButton, &QPushButton::clicked, this, &MainWindow::on_myAccountButton_clicked);
    connect(ui->myGameHistoryButton, &QPushButton::clicked, this, &MainWindow::on_myGameHistoryButton_clicked);
    connect(ui->resetPasswordButton, &QPushButton::clicked, this, &MainWindow::on_resetPasswordButton_clicked);
    connect(ui->backButtonReset, &QPushButton::clicked, this, &MainWindow::on_backButtonReset_clicked);
    connect(ui->backButtonAccount, &QPushButton::clicked, this, &MainWindow::on_backButtonAccount_clicked);
    connect(ui->changePasswordButton, &QPushButton::clicked, this, &MainWindow::on_changePasswordButton_clicked);
    connect(ui->logoutButtonAccount, &QPushButton::clicked, this, &MainWindow::on_logoutButtonAccount_clicked);
    connect(ui->startGameButton, &QPushButton::clicked, this, &MainWindow::on_startGameButton_clicked);
    connect(ui->backButtonAiPage, &QPushButton::clicked, this, &MainWindow::on_backButtonAiPage_clicked);
    connect(ui->resetGameboardButton, &QPushButton::clicked, this, &MainWindow::on_resetGameboardButton_clicked);
    connect(ui->backButtonGamePage, &QPushButton::clicked, this, &MainWindow::on_backButtonGamePage_clicked);
    connect(ui->replayGameButton, &QPushButton::clicked, this, &MainWindow::on_replayGameButton_clicked);
    connect(ui->deleteGameButton, &QPushButton::clicked, this, &MainWindow::on_deleteGameButton_clicked);
    connect(ui->backButtonHistory, &QPushButton::clicked, this, &MainWindow::on_backButtonHistory_clicked);

    // --- Game Board Button Connections ---
    connect(ui->pushButton_00, &QPushButton::clicked, this, &MainWindow::handleBoardClick);
    connect(ui->pushButton_01, &QPushButton::clicked, this, &MainWindow::handleBoardClick);
    connect(ui->pushButton_02, &QPushButton::clicked, this, &MainWindow::handleBoardClick);
    connect(ui->pushButton_10, &QPushButton::clicked, this, &MainWindow::handleBoardClick);
    connect(ui->pushButton_11, &QPushButton::clicked, this, &MainWindow::handleBoardClick);
    connect(ui->pushButton_12, &QPushButton::clicked, this, &MainWindow::handleBoardClick);
    connect(ui->pushButton_20, &QPushButton::clicked, this, &MainWindow::handleBoardClick);
    connect(ui->pushButton_21, &QPushButton::clicked, this, &MainWindow::handleBoardClick);
    connect(ui->pushButton_22, &QPushButton::clicked, this, &MainWindow::handleBoardClick);

    // --- GameLogic Signal Connections ---
    connect(gameLogic, &GameLogic::boardChanged, this, &MainWindow::onBoardChanged);
    connect(gameLogic, &GameLogic::gameEnded, this, &MainWindow::onGameEnded);
    connect(gameLogic, &GameLogic::currentPlayerChanged, this, &MainWindow::onCurrentPlayerChanged);

    // --- Replay Timer Connection ---
    connect(replayTimer, &QTimer::timeout, this, &MainWindow::replayNextMove);
}

void MainWindow::handleShowPassword()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (!button) return;

    QString buttonName = button->objectName();
    QList<QLineEdit*> targetFields;

    if (buttonName == "showLoginPasswordButton") {
        targetFields.append(this->findChild<QLineEdit*>("loginPasswordLineEdit"));
    } else if (buttonName == "showSignupPasswordButton") {
        targetFields.append(this->findChild<QLineEdit*>("signupPasswordLineEdit"));
    } else if (buttonName == "showResetPasswordButton") {
        targetFields.append(this->findChild<QLineEdit*>("resetNewPasswordLineEdit"));
        targetFields.append(this->findChild<QLineEdit*>("resetConfirmPasswordLineEdit"));
    }

    if (targetFields.isEmpty() || !targetFields.first()) return;

    bool showPassword = (targetFields.first()->echoMode() == QLineEdit::Password);
    for (QLineEdit* lineEdit : targetFields) {
        if (lineEdit) {
            lineEdit->setEchoMode(showPassword ? QLineEdit::Normal : QLineEdit::Password);
        }
    }

    if (buttonName == "showResetPasswordButton") {
        button->setText(showPassword ? "Hide Passwords" : "Show Passwords");
    } else {
        button->setText(showPassword ? "Hide Password" : "Show Password");
    }
}

void MainWindow::on_registerButton_clicked()
{
    QString username = ui->signupUsernameLineEdit->text();
    QString password = ui->signupPasswordLineEdit->text();
    QString email = ui->signupEmailLineEdit->text();
    QString firstName = ui->signupFirstNameLineEdit->text();
    QString lastName = ui->signupLastNameLineEdit->text();

    if (username.isEmpty() || password.isEmpty() || email.isEmpty()) {
        Utils::showStyledMessageBox(this, "Signup Failed", "Username, Email, and Password are required.", true);
        return;
    }

    if (dbManager->registerUser(username, password, email, firstName, lastName)) {
        Utils::showStyledMessageBox(this, "Success!", "Your account has been created. You can now log in.");
        ui->stackedWidget->setCurrentWidget(ui->page_0_login);

        ui->signupUsernameLineEdit->clear();
        ui->signupPasswordLineEdit->clear();
        ui->signupEmailLineEdit->clear();
        ui->signupFirstNameLineEdit->clear();
        ui->signupLastNameLineEdit->clear();
    } else {
        Utils::showStyledMessageBox(this, "Signup Failed", "This username or email may already be taken. Please try again.", true);
    }
}

void MainWindow::on_loginButton_clicked() {
    if (m_loginInProgress) return;
    m_loginInProgress = true;
    ui->loginButton->setEnabled(false);

    QString usernameOrEmail = ui->loginUsernameLineEdit->text();
    QString password = ui->loginPasswordLineEdit->text();

    if (usernameOrEmail.isEmpty() || password.isEmpty()) {
        Utils::showStyledMessageBox(this, "Login Failed", "Please enter your identifier and password.", true);
        ui->loginButton->setEnabled(true);
        m_loginInProgress = false;
        return;
    }

    if (dbManager->authenticateUser(usernameOrEmail, password)) {
        QVariantMap userInfo = dbManager->getUserInfo(usernameOrEmail);
        currentUser = userInfo["username"].toString();

        QSettings settings("YourCompanyName", "TicTacToe");
        if (ui->rememberMeCheckBox->isChecked()) {
            settings.setValue("rememberedIdentifier", usernameOrEmail);
        } else {
            settings.remove("rememberedIdentifier");
        }

        ui->stackedWidget->setCurrentWidget(ui->page_1_main);
        ui->loginPasswordLineEdit->clear();

        QString welcomeMessage = "Welcome, " + currentUser + "!";
        QTimer::singleShot(100, this, [this, welcomeMessage]() {
            if (isMessageBoxActive) return;
            isMessageBoxActive = true;
            Utils::showStyledMessageBox(this, "Login Successful", welcomeMessage);
            isMessageBoxActive = false;
        });
    } else {
        Utils::showStyledMessageBox(this, "Login Failed", "Invalid identifier or password.", true);
        ui->loginPasswordLineEdit->clear();
    }

    QTimer::singleShot(500, this, [this](){
        ui->loginButton->setEnabled(true);
        m_loginInProgress = false;
    });
}

void MainWindow::updateAccountInfoUI(const QVariantMap& userInfo) {
    ui->accountFirstNameLabel->setText(userInfo["firstName"].toString());
    ui->accountLastNameLabel->setText(userInfo["lastName"].toString());
    ui->accountUsernameLabel->setText(userInfo["username"].toString());
    ui->accountEmailLabel->setText(userInfo["email"].toString());
}

void MainWindow::on_signupLoginPageButton_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->page_2_signup);
    ui->signupUsernameLineEdit->clear();
    ui->signupPasswordLineEdit->clear();
    ui->signupEmailLineEdit->clear();
    ui->signupFirstNameLineEdit->clear();
    ui->signupLastNameLineEdit->clear();
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
    }
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

// --- MODIFIED: LOGOUT NOW CLEARS THE "REMEMBER ME" SETTING ---
void MainWindow::on_logoutButtonAccount_clicked()
{
    QSettings settings("YourCompanyName", "TicTacToe");
    settings.remove("rememberedIdentifier"); // Clear the auto-login setting

    currentUser.clear();
    ui->stackedWidget->setCurrentWidget(ui->page_0_login);
    ui->loginButton->setEnabled(true);
    m_loginInProgress = false;
}

void MainWindow::on_resetGameboardButton_clicked() {
    if (m_isReplayMode) {
        replayTimer->stop();
        replayIndex = 0;
        resetBoardUI();
        disableGameboardUI();
        replayTimer->start(800);
        ui->gameStatusLabel->setText("Replaying Game (Restarted)...");
    } else {
        gameLogic->resetGame();
        resetBoardUI();
        ui->gameStatusLabel->setText("Player X's Turn");
    }
}

void MainWindow::on_backButtonGamePage_clicked() {
    if (replayTimer->isActive()) {
        replayTimer->stop();
        replayMoves.clear();
        replayIndex = 0;
    }
    m_isReplayMode = false;
    ui->stackedWidget->setCurrentWidget(ui->page_1_main);
}

void MainWindow::on_playerVsPlayerButton_clicked() {
    m_isReplayMode = false;
    gameLogic->startGame(false, "");
    resetBoardUI();
    ui->gameStatusLabel->setText("Player X's Turn");
    ui->stackedWidget->setCurrentWidget(ui->page_5_gameboard);
}

void MainWindow::on_playerVsAiButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_ai_difficulty_selection);
}

void MainWindow::on_startGameButton_clicked()
{
    m_isReplayMode = false;
    QString aiDifficulty;
    if (ui->easyRadioButton_2->isChecked()) {
        aiDifficulty = "easy";
    } else if (ui->mediumRadioButton->isChecked()) {
        aiDifficulty = "medium";
    } else {
        aiDifficulty = "hard";
    }

    gameLogic->startGame(true, aiDifficulty);
    resetBoardUI();
    ui->gameStatusLabel->setText("Player X's Turn");
    ui->stackedWidget->setCurrentWidget(ui->page_5_gameboard);
}

void MainWindow::on_backButtonAiPage_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_1_main);
}

void MainWindow::handleBoardClick() {
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    if (!clickedButton || !clickedButton->text().isEmpty()) {
        return;
    }

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
            if(playerChar == 'X') button->setStyleSheet("color: #89b4fa;"); // Use dark theme blue
            else button->setStyleSheet("color: #f38ba8;"); // Use dark theme red
        }
        replayIndex++;
    } else {
        replayTimer->stop();
        Utils::showStyledMessageBox(this, "Replay Finished", "The game replay has concluded.");
        disableGameboardUI();
    }
}

void MainWindow::on_replayGameButton_clicked() {
    QListWidgetItem *selectedItem = ui->gameHistoryListWidget->currentItem();
    if (selectedItem) {
        int gameId = selectedItem->data(Qt::UserRole).toInt();
        QString movesString = dbManager->getGameMoves(gameId);
        if (!movesString.isEmpty()) {
            m_isReplayMode = true;
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
        button->setStyleSheet((player == Board::PLAYER_X) ? "color: #89b4fa;" : "color: #f38ba8;"); // Use dark theme colors
        button->setEnabled(false);
    }
}

void MainWindow::onGameEnded(const QString& winner, const QStringList& moves) {
    if (!m_isReplayMode) {
        Utils::showStyledMessageBox(this, "Game Over", winner);
        QString player2Name = gameLogic->isVsAI() ? "AI" : "Player O";
        dbManager->saveGameHistory(currentUser, player2Name, winner, moves);
    }
    disableGameboardUI();
}

void MainWindow::onCurrentPlayerChanged(int player) {
    if (!m_isReplayMode) {
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
        disableGameboardUI();
    }
}

void MainWindow::updateGameboardUI() {
    // This method is not currently used but is kept for potential future use.
}
