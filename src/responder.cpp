#include "responder.h"

Responder::Responder(QTcpSocket *socket, QObject *parent): QObject(parent)
{

    messenger = new Messenger;
    m_incomingData = new QByteArray;
    m_responderSocket = new QTcpSocket;
    m_responderSocket = socket;
    qDebug()<<"new responder created";

    connect(m_responderSocket,SIGNAL(readyRead()),this,SLOT(readIncomingData()));
    connect(messenger,SIGNAL(sendMessage(QByteArray)),this,SLOT(sendData(QByteArray)));
    connect(m_responderSocket,SIGNAL(error(QAbstractSocket::SocketError)), this,SLOT(error(QAbstractSocket::SocketError)));

}
void Responder::startCommunication()
{
    qDebug()<<"responder !!";
}
void Responder::readIncomingData()
{
    *m_incomingData = m_responderSocket->readAll();
    messenger->displayMessage("Received : "+*m_incomingData);

    qDebug()<<"incommingData !";
    qDebug()<<m_incomingData;
}
void Responder::sendData(QByteArray data)
{
    if(m_responderSocket->state() == QAbstractSocket::ConnectedState)
    {
        messenger->displayMessage("Sent : "+data);
        m_responderSocket->write(data);
    }
    else messenger->displayMessage("!!!-"+data+"-!!!\n|Couldn't send message : Not connected.");
}
void Responder::error(QAbstractSocket::SocketError error)
{
    qDebug()<<error;

    if(error == QAbstractSocket::RemoteHostClosedError)
    {

        messenger->displayMessage("The server has disconnected...");

    }
}

Responder::~Responder()
{

}
