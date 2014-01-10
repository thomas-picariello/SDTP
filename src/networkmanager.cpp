#include "networkmanager.h"

NetworkManager::NetworkManager(QTcpSocket *socket, QObject *parent): QObject(parent)
{
    m_Socket = new QTcpSocket;
    m_Socket = socket;

   // contact = new Contact("unknown");

    m_MessengerWindow = new MessengerWindow;

    QString ip = m_Socket->peerAddress().toString();
    qDebug()<<ip;

    voip = new VoIP;

    m_MessengerWindow->displayMessage(Message("Connected to : "+ip, Message::SERVICE));

    connect(m_MessengerWindow,SIGNAL(callContact()),
            this, SLOT(call()));


}
void NetworkManager::call()
{

    voip->call(Contact());
}

void NetworkManager::readIncomingData()
{

}

void NetworkManager::sendData(QByteArray data)
{

}
void NetworkManager::error(QAbstractSocket::SocketError error)
{

}

NetworkManager::~NetworkManager()
{


}
