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
// REMOVED: class EmailManager;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handleShowPassword();
    void on_loginButton_clicked();
    void on_signupLoginPageButton_clicked();
    void on_noWorriesButton_clicked();
    void on_registerButton_clicked();
    void on_loginButtonSignupPage_clicked();
    void on_playerVsAiButton_clicked();
    void on_playerVsPlayerButton_clicked();
    void on_myAccountButton_clicked();
    void on_myGameHistoryButton_clicked();
    void on_resetPasswordButton_clicked();
    void on_backButtonReset_clicked();
    void on_backButtonAccount_clicked();
    void on_changePasswordButton_clicked();
    void on_logoutButtonAccount_clicked();
    void on_resetGameboardButton_clicked();
    void on_backButtonGamePage_clicked();
    void on_replayGameButton_clicked();
    void on_deleteGameButton_clicked();
    void on_backButtonHistory_clicked();
    void on_startGameButton_clicked();
    void on_backButtonAiPage_clicked();

    // --- REMOVED VERIFICATION FLOW SLOTS ---
    // void on_verifyButton_clicked();
    // void onEmailSent(bool success, const QString& message);

    // Game Logic Slots
    void handleBoardClick();
    void replayNextMove();
    void onBoardChanged(int row, int col, int player);
    void onGameEnded(const QString& winner, const QStringList& moves);
    void onCurrentPlayerChanged(int player);

private:
    void setupConnections();
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
    // REMOVED: EmailManager *emailManager;
    QString currentUser;
    QTimer *replayTimer;
    QStringList replayMoves;
    int replayIndex;
    bool isMessageBoxActive;
    bool m_loginInProgress;
    bool m_isReplayMode;
    // --- REMOVED VERIFICATION MEMBER VARIABLES ---
    // QVariantMap registrationData;
    // QString verificationCode;
};

#endif // MAINWINDOW_H
