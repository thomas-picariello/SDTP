#include "svoip.h"

SVoIP::SVoIP(QObject *parent) : QObject(parent)
{
    mWindow = new MainWindow();

    qint16 listenPort = QSettings().value("Settings/port").toInt();

    mListener = new QTcpServer();
    mListener->listen(QHostAddress::Any, listenPort);

    connect(mListener, SIGNAL(newConnection()),this,SLOT(acceptConnection()));
}
void SVoIP::acceptConnection()
{
    m_responderList.append(Responder(mListener->nextPendingConnection()));

}

SVoIP::~SVoIP()
{
    delete mWindow, mListener;
}
