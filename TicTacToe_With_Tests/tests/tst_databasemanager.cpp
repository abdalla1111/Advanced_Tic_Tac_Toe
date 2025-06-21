#include "tst_databasemanager.h"
#include "DatabaseManager.h"
#include <QFile> // Required for QFile::remove
#include <QSqlDatabase> // Required for QSqlDatabase
#include <QSqlQuery> // Required for QSqlQuery
#include <QSqlError> // Required for QSqlError
#include <QDebug> // Required for qDebug()
#include <QTest>  // Explicitly include QTest for QTest::qWait

void TestDatabaseManager::initTestCase()
{
    // Clean up any old test database file to ensure a fresh start
    QFile::remove("test_users.db"); //
    // Ensure no lingering connection that might prevent file deletion immediately after a failed run
    if (QSqlDatabase::contains("qt_sql_default_connection")) { //
        QSqlDatabase::removeDatabase("qt_sql_default_connection"); //
    }
}

void TestDatabaseManager::cleanupTestCase()
{
    // Clean up the test database file after all tests are done
    QFile::remove("test_users.db"); //
    // Ensure no lingering connection from test suite
    if (QSqlDatabase::contains("qt_sql_default_connection")) { //
        QSqlDatabase::removeDatabase("qt_sql_default_connection"); //
    }
}

void TestDatabaseManager::init()
{
    // Ensure the database file is removed before each test to guarantee isolation.
    // This makes each test run on a completely fresh database.
    QFile::remove("test_users.db"); //

    // Remove the default connection if it exists from a previous test.
    // DatabaseManager uses the default connection name if not specified.
    if (QSqlDatabase::contains("qt_sql_default_connection")) { //
        QSqlDatabase::removeDatabase("qt_sql_default_connection"); //
    }
}

void TestDatabaseManager::cleanup()
{
    // No explicit cleanup needed here beyond what init() does for the next test.
    // The DatabaseManager object created in each test function will be destroyed
    // when the function returns, closing its connection.
}

void TestDatabaseManager::testInitializeDatabase()
{
    DatabaseManager dbManager(this, "test_users.db"); // Use the new constructor
    QVERIFY(dbManager.initializeDatabase()); //

    // Verify tables exist by trying to query them using the same connection used by dbManager
    QSqlDatabase db = QSqlDatabase::database(); // Gets the default connection, which dbManager uses
    QVERIFY(db.isOpen()); //
    QSqlQuery query(db); //
    QVERIFY(query.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='users';")); //
    QVERIFY(query.next()); // Should find the 'users' table
    QCOMPARE(query.value(0).toString(), "users"); //

    QVERIFY(query.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='game_history';")); //
    QVERIFY(query.next()); // Should find the 'game_history' table
    QCOMPARE(query.value(0).toString(), "game_history"); //
}

void TestDatabaseManager::testRegisterUser_success()
{
    DatabaseManager dbManager(this, "test_users.db"); // Use new constructor
    dbManager.initializeDatabase(); //
    QVERIFY(dbManager.registerUser("testuser1", "pass123", "John", "Doe")); //

    // Verify user exists by trying to authenticate
    QVERIFY(dbManager.authenticateUser("testuser1", "pass123")); //
}

void TestDatabaseManager::testRegisterUser_duplicate()
{
    DatabaseManager dbManager(this, "test_users.db"); // Use new constructor
    dbManager.initializeDatabase(); //
    QVERIFY(dbManager.registerUser("testuser_dup", "pass", "A", "B")); //
    QVERIFY(!dbManager.registerUser("testuser_dup", "another_pass", "C", "D")); // Should fail
}

void TestDatabaseManager::testAuthenticateUser_success()
{
    DatabaseManager dbManager(this, "test_users.db"); // Use new constructor
    dbManager.initializeDatabase(); //
    dbManager.registerUser("authuser", "securepass", "Auth", "User"); //
    QVERIFY(dbManager.authenticateUser("authuser", "securepass")); //
}

void TestDatabaseManager::testAuthenticateUser_failure()
{
    DatabaseManager dbManager(this, "test_users.db"); // Use new constructor
    dbManager.initializeDatabase(); //
    dbManager.registerUser("wrongpassuser", "correctpass", "Wrong", "Pass"); //
    QVERIFY(!dbManager.authenticateUser("wrongpassuser", "incorrectpass")); // Wrong password
    QVERIFY(!dbManager.authenticateUser("nonexistentuser", "anypass"));    // Non-existent user
}

void TestDatabaseManager::testResetUserPassword_success()
{
    DatabaseManager dbManager(this, "test_users.db"); // Use new constructor
    dbManager.initializeDatabase(); //
    dbManager.registerUser("resetuser", "oldpass", "Reset", "User"); //
    QVERIFY(dbManager.authenticateUser("resetuser", "oldpass")); // Verify old pass works

    QVERIFY(dbManager.resetUserPassword("resetuser", "newpass")); //
    QVERIFY(!dbManager.authenticateUser("resetuser", "oldpass")); // Old pass should not work
    QVERIFY(dbManager.authenticateUser("resetuser", "newpass"));  // New pass should work
}

void TestDatabaseManager::testResetUserPassword_nonExistentUser()
{
    DatabaseManager dbManager(this, "test_users.db"); // Use new constructor
    dbManager.initializeDatabase(); //
    QVERIFY(!dbManager.resetUserPassword("nonexistent_reset_user", "some_pass")); // Should fail
}

void TestDatabaseManager::testGetUserInfo()
{
    DatabaseManager dbManager(this, "test_users.db"); // Use new constructor
    dbManager.initializeDatabase(); //
    dbManager.registerUser("infoUser", "infoPass", "First", "Last"); //

    QVariantMap userInfo = dbManager.getUserInfo("infoUser"); //
    QCOMPARE(userInfo["firstName"].toString(), "First"); //
    QCOMPARE(userInfo["lastName"].toString(), "Last"); //
    QCOMPARE(userInfo["username"].toString(), "infoUser"); //

    QVariantMap emptyInfo = dbManager.getUserInfo("nonExistentInfoUser"); //
    QVERIFY(emptyInfo.isEmpty()); // Should be empty for non-existent user
}

void TestDatabaseManager::testSaveAndLoadGameHistory()
{
    DatabaseManager dbManager(this, "test_users.db"); // Use new constructor
    dbManager.initializeDatabase(); //
    dbManager.registerUser("player1_hist", "pass", "P1", "H"); //
    dbManager.registerUser("player2_hist", "pass", "P2", "H"); //

    QStringList moves1 = {"0:0:X", "1:1:O", "0:1:X"}; //
    QVERIFY(dbManager.saveGameHistory("player1_hist", "player2_hist", "Player1 Wins", moves1)); //

    // Add a small delay to ensure distinct timestamps for robust ordering in SQLite
    QTest::qWait(500); // Increased from 200ms to 500ms, as per latest discussion.

    QStringList moves2 = {"0:0:X", "0:1:O", "0:2:X", "1:0:O", "1:2:X", "1:1:O", "2:1:X", "2:0:O", "2:2:X"}; //
    QVERIFY(dbManager.saveGameHistory("player1_hist", "AI", "Draw", moves2)); //

    QList<QVariantMap> history = dbManager.loadGameHistory("player1_hist"); //
    QCOMPARE(history.size(), 2); //

    // Verify the latest game first (DESC order by timestamp)
    QCOMPARE(history[0]["player1"].toString(), "player1_hist"); //
    QCOMPARE(history[0]["player2"].toString(), "AI"); //
    QCOMPARE(history[0]["result"].toString(), "Draw"); //
    QCOMPARE(history[0]["moves"].toString(), moves2.join(",")); //

    // Verify the older game
    QCOMPARE(history[1]["player1"].toString(), "player1_hist"); //
    QCOMPARE(history[1]["player2"].toString(), "player2_hist"); //
    QCOMPARE(history[1]["result"].toString(), "Player1 Wins"); //
    QCOMPARE(history[1]["moves"].toString(), moves1.join(",")); //
}

void TestDatabaseManager::testLoadGameHistory_noHistory()
{
    DatabaseManager dbManager(this, "test_users.db"); // Use new constructor
    dbManager.initializeDatabase(); //
    dbManager.registerUser("nohistoryuser", "pass", "No", "History"); //

    QList<QVariantMap> history = dbManager.loadGameHistory("nohistoryuser"); //
    QVERIFY(history.isEmpty()); //
}

void TestDatabaseManager::testDeleteGameHistory()
{
    DatabaseManager dbManager(this, "test_users.db"); // Use new constructor
    dbManager.initializeDatabase(); //
    dbManager.registerUser("player_del", "pass", "", ""); //

    dbManager.saveGameHistory("player_del", "AI", "Win", {"0:0:X"}); //
    QTest::qWait(500); // Small delay to ensure distinct timestamp
    dbManager.saveGameHistory("player_del", "AI", "Loss", {"1:1:X"}); //

    QList<QVariantMap> historyBeforeDelete = dbManager.loadGameHistory("player_del"); //
    QCOMPARE(historyBeforeDelete.size(), 2); //

    int gameIdToDelete = historyBeforeDelete[0]["id"].toInt(); //
    QVERIFY(dbManager.deleteGameHistory(gameIdToDelete)); //

    QList<QVariantMap> historyAfterDelete = dbManager.loadGameHistory("player_del"); //
    QCOMPARE(historyAfterDelete.size(), 1); //
    QVERIFY(historyAfterDelete[0]["id"].toInt() != gameIdToDelete); //
}

void TestDatabaseManager::testGetGameMoves()
{
    DatabaseManager dbManager(this, "test_users.db"); // Use new constructor
    dbManager.initializeDatabase(); //
    dbManager.registerUser("player_moves", "pass", "", ""); //

    QStringList testMoves = {"0:0:X", "1:0:O", "0:1:X", "1:1:O", "0:2:X"}; //
    dbManager.saveGameHistory("player_moves", "AI", "Player_moves Wins", testMoves); //

    QList<QVariantMap> history = dbManager.loadGameHistory("player_moves"); //
    QVERIFY(!history.isEmpty()); //
    int gameId = history[0]["id"].toInt(); //

    QString fetchedMoves = dbManager.getGameMoves(gameId); //
    QCOMPARE(fetchedMoves, testMoves.join(",")); //

    QString emptyMoves = dbManager.getGameMoves(99999); //
    QVERIFY(emptyMoves.isEmpty()); //
}
