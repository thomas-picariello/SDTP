#include "responder.h"

Responder::Responder(quintptr socketDescriptor, QObject *parent) :
    QThread(parent), mSocketDescriptor(socketDescriptor)
{
    mQuitFlag = false;
    mSocket = 0;
}

Responder::~Responder(){
    if(mSocket != 0){
        mSocket->close();
    }
}

void Responder::onReadyRead(){
    //Called When data are recieved
    QByteArray data = mSocket->readAll();
    data.prepend("Server echo: ");
    qDebug() << data;
    mSocket->write(QByteArray());
}
void Responder::onDisconnect(){
    emit disconnected(getSocketDescriptor());
}

quintptr Responder::getSocketDescriptor() const{
    return mSocketDescriptor;
}
void Responder::closeConnection(){
    mQuitFlag = true;
    if(mSocket != 0){
        mSocket->close();
    }
    emit disconnected(getSocketDescriptor());
    exit();
}


void Responder::run(){
    mSocket = new QTcpSocket();
    mSocket->setSocketDescriptor(mSocketDescriptor,
                                QTcpSocket::ConnectedState,
                                QTcpSocket::ReadWrite);

    connect(mSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SIGNAL(error(QAbstractSocket::SocketError)));
    connect(mSocket, SIGNAL(disconnected()),
            this, SLOT(onDisconnect()));
    connect(mSocket, SIGNAL(readyRead()),
            this, SLOT(onReadyRead()));

    qDebug() << "Server thread connected to:" << mSocket->peerAddress().toString();
    mSocket->write("Hello from server :)");
    while(!mQuitFlag){
        msleep(50);
    }
    closeConnection();
}
