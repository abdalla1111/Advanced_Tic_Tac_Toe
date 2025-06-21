#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include <QStringList>
#include <QDebug>
#include <QVariantMap>

// REMOVE THIS LINE:
// class TestDatabaseManager; // Only forward declare classes that are friends *and* defined elsewhere.
// TestDatabaseManager is defined in its own header.

class DatabaseManager : public QObject // THIS is the class definition
{
    Q_OBJECT

    // This is fine: grants access to the test class, which is defined in another file.
    friend class TestDatabaseManager;

public:
    // Default constructor (for application use, typically "users.db")
    explicit DatabaseManager(QObject *parent = nullptr);
    // Overloaded constructor for specific database file (useful for testing)
    explicit DatabaseManager(QObject *parent, const QString& dbFileName);

    ~DatabaseManager();

    bool initializeDatabase();
    bool registerUser(const QString &username, const QString &password, const QString &firstName, const QString &lastName);
    bool authenticateUser(const QString &username, const QString &password);
    bool resetUserPassword(const QString &username, const QString &newPassword);
    bool saveGameHistory(const QString &player1, const QString &player2, const QString &result, const QStringList &moves);
    QList<QVariantMap> loadGameHistory(const QString &username);
    bool deleteGameHistory(int gameId);
    QVariantMap getUserInfo(const QString& username);
    QString getGameMoves(int gameId);

private:
    QSqlDatabase db;
};

#endif // DATABASEMANAGER_H
