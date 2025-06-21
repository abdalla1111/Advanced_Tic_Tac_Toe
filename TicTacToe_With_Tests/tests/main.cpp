#include <QtTest>
#include <QApplication> // <--- Change this back to QApplication

// Include the test headers
#include "tst_testboard.h"
#include "tst_aiplayer.h"
#include "tst_gamelogic.h"
#include "tst_databasemanager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv); // <--- Use QApplication here
    int status = 0;

    { TestBoard tc; status |= QTest::qExec(&tc, argc, argv); }
    { TestAIPlayer tc; status |= QTest::qExec(&tc, argc, argv); }
    { TestGameLogic tc; status |= QTest::qExec(&tc, argc, argv); }
    { TestDatabaseManager tc; status |= QTest::qExec(&tc, argc, argv); }

    return status;
}
