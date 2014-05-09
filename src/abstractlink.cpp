#include "abstractlink.h"

AbstractLink::AbstractLink(QString ip,PacketAgent* agent)
{
//    m_State = OFFLINE;
//    m_Socket = new QTcpSocket;
//    m_Agent = agent;
//    HostListLength = m_Contact->hostsList()->length();
//    connect(m_Socket,SIGNAL(connected()),this,SLOT(onConnected()));
//    connect(m_Socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(onSocketError(QAbstractSocket::SocketError)));


//    tryConnect();
}
AbstractLink::State AbstractLink::state(){

    return AbstractLink::State::Error;
}
void AbstractLink::write(QByteArray data){

}
void AbstractLink::read(){
    m_Agent->incomingdata(m_Socket->readAll());

}
void AbstractLink::onConnected(){

}
void AbstractLink::tryConnect(){

}
void AbstractLink::onConnectionError(){

}
PacketAgent* AbstractLink::getagent(){

    return NULL;
}
AbstractLink::~AbstractLink(){

}
