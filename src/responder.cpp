#include "responder.h"

Responder::Responder(QTcpSocket *socket)
{

    m_responderSocket = socket;
    qDebug()<<"new responder created";

    //connect(m_responderSocket,SIGNAL(readyRead()),this,SLOT(readIncomingData()));
}
void Responder::startCommunication()
{
    qDebug()<<"responder !!";
}
void Responder::readIncomingData()
{
    qDebug()<<m_responderSocket->readAll();
}

Responder::~Responder()
{

}
