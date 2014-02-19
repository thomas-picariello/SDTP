#include "contactlistwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("FHNW-ICS");
    QCoreApplication::setApplicationName("SVoIP");
    QCoreApplication::setApplicationVersion("0.1");


    ContactListWindow contactListWindow;

    return a.exec();
}
