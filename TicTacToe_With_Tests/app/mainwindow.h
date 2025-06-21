#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QVariantMap>

// Forward declarations
namespace Ui {
class MainWindow;
}
class DatabaseManager;
class GameLogic;
class QPushButton;
class QLineEdit;
class QTimer;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Page 0: Login
    void on_loginButton_clicked();
    void on_signupLoginPageButton_clicked();
    void on_showLoginPasswordCheckBox_toggled(bool checked);
    void on_noWorriesButton_clicked();

    // Page 2: Signup
    void on_registerButton_clicked();
    void on_showSignupPasswordCheckBox_toggled(bool checked);
    void on_loginButtonSignupPage_clicked();

    // Page 1: Main Menu
    void on_playerVsAiButton_clicked();
    void on_playerVsPlayerButton_clicked();
    void on_myAccountButton_clicked();
    void on_myGameHistoryButton_clicked();

    // Page 3: Password Reset
    void on_resetPasswordButton_clicked();
    void on_showResetNewPasswordCheckBox_toggled(bool checked);
    void on_backButtonReset_clicked();

    // Page 4: Personal Info
    void on_backButtonAccount_clicked();
    void on_changePasswordButton_clicked();
    void on_logoutButtonAccount_clicked();

    // Page 5: Game Board
    void handleBoardClick();
    void on_resetGameboardButton_clicked();
    void on_backButtonGamePage_clicked();

    // Page 6: Game History
    void on_replayGameButton_clicked();
    void on_deleteGameButton_clicked();
    void on_backButtonHistory_clicked();
    void replayNextMove();

    // Game Logic Slots
    void onBoardChanged(int row, int col, int player);
    void onGameEnded(const QString& winner, const QStringList& moves);
    void onCurrentPlayerChanged(int player);

private:
    void setupConnections();
    void togglePasswordVisibility(QLineEdit* lineEdit, bool visible);
    void updateAccountInfoUI(const QVariantMap& userInfo);
    void loadGameHistoryUI();
    void resetBoardUI();
    QPushButton* getButton(int row, int col);
    void disableGameboardUI();
    void enableGameboardUI();
    void updateGameboardUI();


    Ui::MainWindow *ui;
    DatabaseManager *dbManager;
    GameLogic *gameLogic;
    QString currentUser;
    QTimer *replayTimer;
    QStringList replayMoves;
    int replayIndex;
    bool isMessageBoxActive;
    bool m_loginInProgress;
    bool m_isReplayMode; // *FIX: Added flag to track replay mode*
};

#endif // MAINWINDOW_H
