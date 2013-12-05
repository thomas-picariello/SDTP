#include "svoip.h"

SVoIP::SVoIP()
{
    mWindow = new MainWindow();
    qint16 listenPort = QSettings().value("Settings/port").toInt();
    //mListener = new Listener(listenPort);
}
SVoIP::~SVoIP()
{
    delete mWindow, mListener;
}
