#include "abstractlink.h"

AbstractLink::AbstractLink(Contact contact)
{
   // mContact = contact;
    connect();
    handshake();

    mAgent = new PacketAgent(QPair<QByteArray,QByteArray>());

    //connect(mSocket,SIGNAL(readyRead()),mAgent,SIGNAL(newdata()));

}
AbstractLink::AbstractLink(QTcpSocket *socket){

    mSocket = socket;
    handshake();
}
AbstractLink::State AbstractLink::state(){

return Error;
}
void AbstractLink::write(QByteArray data){

    mSocket->write(data);
}
QByteArray AbstractLink::read(){


return mSocket->readAll();
}
void AbstractLink::connect(){

}
void AbstractLink::handshake(){

}
AbstractLink::~AbstractLink(){

}
