#include "svoip.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("FHNW-ICS");
    QCoreApplication::setApplicationName("P5-SVoIP");
    SVoIP svoip;

    return a.exec();
}
