#include "svoip.h"

SVoIP::SVoIP(QObject *parent) : QObject(parent){
    mWindow = new ContactListWindow();

    qint16 listenPort = QSettings().value("Settings/port").toInt();

    mListener = new QTcpServer();
    mListener->listen(QHostAddress::Any, listenPort);

    connect(mListener, SIGNAL(newConnection()),this,SLOT(acceptConnection()));
}
void SVoIP::acceptConnection(){
  //  m_responderList.append(new Responder(mListener->nextPendingConnection()));
    m_ManagerList.append(new NetworkManager(mListener->nextPendingConnection()));

}

SVoIP::~SVoIP(){
    delete mWindow, mListener;
}
