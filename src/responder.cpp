#include "responder.h"

Responder::Responder(QTcpSocket *socket, QObject *parent): QObject(parent)
{

    m_messenger_window = new MessengerWindow;
    m_incomingData = new QByteArray;
    m_responderSocket = new QTcpSocket;
    m_responderSocket = socket;
    //qDebug()<<"new responder created";

    connect(m_responderSocket,SIGNAL(readyRead()),this,SLOT(readIncomingData()));
    connect(m_messenger_window,SIGNAL(sendMessage(QByteArray)),this,SLOT(sendData(QByteArray)));
    connect(m_responderSocket,SIGNAL(error(QAbstractSocket::SocketError)), this,SLOT(error(QAbstractSocket::SocketError)));

}
void Responder::startCommunication()
{
    //qDebug()<<"responder !!";
}
void Responder::readIncomingData()
{
    *m_incomingData = m_responderSocket->readAll();
    m_messenger_window->displayMessage(Message(QString(*m_incomingData), Message::RECIEVED));

    //qDebug() << "incommingData !" << *m_incomingData;
}
void Responder::sendData(QByteArray data)
{
    if(m_responderSocket->state() == QAbstractSocket::ConnectedState)
    {
        m_messenger_window->displayMessage(Message(QString(data), Message::SENT));
        m_responderSocket->write(data);
    }
    else m_messenger_window->displayMessage(Message("Couldn't send message : Not connected.", Message::ERR));
}
void Responder::error(QAbstractSocket::SocketError error)
{
    qDebug()<<error;

    if(error == QAbstractSocket::RemoteHostClosedError){
        m_messenger_window->displayMessage(Message("The server has disconnected...", Message::ERR));

    }
}

Responder::~Responder(){
    delete m_responderSocket;
    delete m_messenger_window;
    delete m_incomingData;
}
