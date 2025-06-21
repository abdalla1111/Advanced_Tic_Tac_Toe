#include "DatabaseManager.h"
#include <QDateTime> // For QDateTime, used for timestamp handling
#include <QDebug>    // For qDebug() for logging and error messages

// Default constructor - uses "users.db"
DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent)
{
    // Check if the default connection exists, if so, use it, otherwise add it.
    // This handles cases where a connection might already be established
    // (e.g., from a previous DatabaseManager instance if not properly removed).
    if (QSqlDatabase::contains()) { // Checks if the default connection ("qt_sql_default_connection") exists
        db = QSqlDatabase::database();
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE"); // Add a new SQLite database connection
    }
    db.setDatabaseName("users.db"); // Set the default database file name
    initializeDatabase(); // Initialize tables if they don't exist
}

// NEW Constructor for testing - accepts a specific database file name
DatabaseManager::DatabaseManager(QObject *parent, const QString& dbFileName) : QObject(parent)
{
    // For testing, it's often good practice to use a unique connection name
    // to avoid conflicts with the default application connection.
    // However, for simplicity and alignment with existing test structure,
    // we'll still use the default connection name if one isn't specified directly.
    // The key here is overriding the `setDatabaseName` to the test file.

    if (QSqlDatabase::contains()) { // Check for the default connection
        db = QSqlDatabase::database();
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE"); // Add a new default connection
    }
    db.setDatabaseName(dbFileName); // Set the provided database file name for this instance
    initializeDatabase(); // Initialize tables if they don't exist
}

DatabaseManager::~DatabaseManager()
{
    if (db.isOpen()) {
        db.close(); // Close the database connection when the object is destroyed
    }
    // Do NOT call QSqlDatabase::removeDatabase() here in the application's destructor.
    // Removing connections explicitly in application code should be done with care,
    // as it can affect other parts of the application if they rely on the same connection.
    // For tests, `init()` and `cleanupTestCase()` in tst_databasemanager.cpp handle removal.
}

bool DatabaseManager::initializeDatabase() {
    if (!db.open()) {
        qDebug() << "Error: connection with database failed:" << db.lastError().text();
        return false;
    }

    QSqlQuery query(db); // Create a QSqlQuery object associated with this database connection
    // Create 'users' table if it doesn't exist
    bool success = query.exec("CREATE TABLE IF NOT EXISTS users ("
                              "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                              "username TEXT UNIQUE NOT NULL," // Username must be unique
                              "password TEXT NOT NULL,"
                              "firstName TEXT,"
                              "lastName TEXT)");
    if (!success) {
        qDebug() << "Error creating users table:" << query.lastError().text();
        return false;
    }

    // Create 'game_history' table if it doesn't exist
    success = query.exec("CREATE TABLE IF NOT EXISTS game_history ("
                         "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                         "player1 TEXT NOT NULL,"
                         "player2 TEXT NOT NULL,"
                         "result TEXT NOT NULL," // Stores game result (e.g., "Player X Wins!", "Draw")
                         "moves TEXT NOT NULL,"   // Stores a comma-separated string of moves
                         "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP)"); // Automatically records insertion time
    if (!success) {
        qDebug() << "Error creating game_history table:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::registerUser(const QString &username, const QString &password, const QString &firstName, const QString &lastName) {
    if (!db.isOpen()) {
        qDebug() << "Database not open.";
        return false;
    }

    QSqlQuery query(db); // Associate query with the current database connection
    query.prepare("INSERT INTO users (username, password, firstName, lastName) VALUES (:u, :p, :f, :l)");
    query.bindValue(":u", username);
    query.bindValue(":p", QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex()); // Hash password
    query.bindValue(":f", firstName);
    query.bindValue(":l", lastName);

    if (!query.exec()) {
        qDebug() << "Failed to register user:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::authenticateUser(const QString &username, const QString &password) {
    if (!db.isOpen()) {
        qDebug() << "Database not open.";
        return false;
    }

    QSqlQuery query(db); // Associate query with the current database connection
    query.prepare("SELECT * FROM users WHERE username = :u AND password = :p");
    query.bindValue(":u", username);
    query.bindValue(":p", QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex()); // Hash input password for comparison

    if (query.exec() && query.next()) { // Execute query and check if any record was found
        return true; // User found and password matches
    }
    qDebug() << "Authentication failed for user:" << username;
    return false; // Authentication failed
}

bool DatabaseManager::resetUserPassword(const QString &username, const QString &newPassword) {
    if (!db.isOpen()) {
        qDebug() << "Database not open.";
        return false;
    }

    QSqlQuery query(db); // Associate query with the current database connection
    query.prepare("UPDATE users SET password = :password WHERE username = :username");
    query.bindValue(":password", QCryptographicHash::hash(newPassword.toUtf8(), QCryptographicHash::Sha256).toHex()); // Hash new password
    query.bindValue(":username", username);

    // Check if query executed successfully AND if any rows were affected (user found)
    if (!query.exec() || query.numRowsAffected() == 0) {
        qDebug() << "Failed to reset password (user may not exist):" << query.lastError().text();
        return false;
    }
    return true; // Password reset successful
}

bool DatabaseManager::saveGameHistory(const QString &player1, const QString &player2, const QString &result, const QStringList &moves) {
    if (!db.isOpen()) {
        qDebug() << "Database not open.";
        return false;
    }

    QSqlQuery query(db); // Associate query with the current database connection
    query.prepare("INSERT INTO game_history (player1, player2, result, moves) VALUES (:p1, :p2, :r, :m)");
    query.bindValue(":p1", player1);
    query.bindValue(":p2", player2);
    query.bindValue(":r", result);
    query.bindValue(":m", moves.join(",")); // Join QStringList into a single comma-separated string

    if (!query.exec()) {
        qDebug() << "Failed to save game history:" << query.lastError().text();
        return false;
    }
    return true; // Game history saved successfully
}

QList<QVariantMap> DatabaseManager::loadGameHistory(const QString &username) {
    QList<QVariantMap> historyList;
    if (!db.isOpen()) {
        qDebug() << "Database not open.";
        return historyList;
    }

    QSqlQuery query(db); // Associate query with the current database connection
    // MODIFIED: Added 'id DESC' to the ORDER BY clause for deterministic sorting
    query.prepare("SELECT id, player1, player2, result, timestamp, moves FROM game_history WHERE player1 = :currentUser OR player2 = :currentUser ORDER BY timestamp DESC, id DESC");
    query.bindValue(":currentUser", username); // Bind current username for filtering history

    if (query.exec()) {
        while (query.next()) { // Iterate through results
            QVariantMap item; // Use QVariantMap to store each history item's data
            item["id"] = query.value("id").toInt();
            item["player1"] = query.value("player1").toString();
            item["player2"] = query.value("player2").toString();
            item["result"] = query.value("result").toString();
            item["timestamp"] = query.value("timestamp").toDateTime();
            item["moves"] = query.value("moves").toString();
            historyList.append(item); // Add item to the list
        }
    } else {
        qDebug() << "Failed to load game history:" << query.lastError().text();
    }
    return historyList;
}

bool DatabaseManager::deleteGameHistory(int gameId) {
    if (!db.isOpen()) {
        qDebug() << "Database not open.";
        return false;
    }

    QSqlQuery query(db); // Associate query with the current database connection
    query.prepare("DELETE FROM game_history WHERE id = :gameId");
    query.bindValue(":gameId", gameId); // Bind the ID of the game to delete

    if (!query.exec()) {
        qDebug() << "Failed to delete game history:" << query.lastError().text();
        return false;
    }
    return true; // Game history deleted successfully
}

QVariantMap DatabaseManager::getUserInfo(const QString& username) {
    QVariantMap userInfo;
    if (!db.isOpen()) {
        qDebug() << "Database not open.";
        return userInfo;
    }

    QSqlQuery query(db); // Associate query with the current database connection
    query.prepare("SELECT firstName, lastName, username FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if (query.exec() && query.next()) { // Execute query and check if a user was found
        userInfo["firstName"] = query.value("firstName").toString();
        userInfo["lastName"] = query.value("lastName").toString();
        userInfo["username"] = query.value("username").toString();
    } else {
        qDebug() << "Failed to get user info:" << query.lastError().text();
    }
    return userInfo;
}

QString DatabaseManager::getGameMoves(int gameId) {
    if (!db.isOpen()) {
        qDebug() << "Database not open.";
        return QString();
    }

    QSqlQuery query(db); // Associate query with the current database connection
    query.prepare("SELECT moves FROM game_history WHERE id = :gameId");
    query.bindValue(":gameId", gameId);

    if (query.exec() && query.next()) { // Execute query and check if a record was found
        return query.value(0).toString(); // Return the 'moves' column value
    } else {
        qDebug() << "Failed to get game moves:" << query.lastError().text();
    }
    return QString(); // Return empty string if not found or failed
}
