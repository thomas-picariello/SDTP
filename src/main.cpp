#include <QApplication>
#include "svoip.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("FHNW-ICS");
    QCoreApplication::setApplicationName("SVoIP");
    QCoreApplication::setApplicationVersion("0.1");


    SVoIP svoip;

    return a.exec();
}
