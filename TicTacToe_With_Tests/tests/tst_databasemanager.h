#ifndef TST_DATABASEMANAGER_H
#define TST_DATABASEMANAGER_H

#include <QObject>
#include <QtTest/QtTest> // Correctly includes QtTest framework
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile> // Required for QFile::remove
#include "DatabaseManager.h" // Include the class we are testing

class TestDatabaseManager : public QObject
{
    Q_OBJECT

private slots:
    // Test case initialization and cleanup
    void initTestCase();    // Called once before any test functions in this class are executed
    void cleanupTestCase(); // Called once after all test functions in this class have been executed
    void init();            // Called before each test function
    void cleanup();         // Called after each test function

    // Tests for Database Initialization
    void testInitializeDatabase();

    // Tests for User Management
    void testRegisterUser_success();
    void testRegisterUser_duplicate();
    void testAuthenticateUser_success();
    void testAuthenticateUser_failure();
    void testResetUserPassword_success();
    void testResetUserPassword_nonExistentUser();
    void testGetUserInfo();

    // Tests for Game History Management
    void testSaveAndLoadGameHistory();
    void testLoadGameHistory_noHistory();
    void testDeleteGameHistory();
    void testGetGameMoves();
};

#endif // TST_DATABASEMANAGER_H
