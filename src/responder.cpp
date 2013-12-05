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
    m_responderSocket->write(data);
}

Responder::~Responder()
{

}
