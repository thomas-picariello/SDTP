#include "tcplink.h"

TcpLink::TcpLink(const QString &host, quint16 port):
    AbstractLink()
{
    m_Socket = new QTcpSocket(this);
    m_Socket->connectToHost(host, port);
    connect(m_Socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(onConnectionError(QAbstractSocket::SocketError)));
}

TcpLink::TcpLink(QTcpSocket *socket):
    AbstractLink(socket)
{

}

QByteArray TcpLink::readAll(){
    return m_Socket->readAll();
}

void TcpLink::write(QByteArray data){

}

void TcpLink::onConnected(){

}

void TcpLink::onConnectionError(QAbstractSocket::SocketError){
    emit error(m_Socket->errorString());
}

TcpLink::~TcpLink(){

}
