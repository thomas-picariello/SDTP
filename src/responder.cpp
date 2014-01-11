#include "responder.h"

Responder::Responder(QTcpSocket *socket, QObject *parent): QObject(parent){
    m_messenger_window = new MessengerWindow;
    m_responderSocket = new QTcpSocket;
    m_responderSocket = socket;

    //qDebug()<<"new responder created";

    connect(m_responderSocket,SIGNAL(error(QAbstractSocket::SocketError)), this,SLOT(error(QAbstractSocket::SocketError)));
}
void Responder::error(QAbstractSocket::SocketError error){
    qDebug()<<error;

    if(error == QAbstractSocket::RemoteHostClosedError){
        m_messenger_window->displayMessage(Message("The server has disconnected...", Message::ERR));
    }
}

Responder::~Responder(){
    delete m_responderSocket;
    delete m_messenger_window;
}
