#include "listener.h"

Listener::Listener(quint16 listenPort, QObject *parent) : QTcpServer(parent){
    listen(QHostAddress::Any, listenPort);
    qDebug() << "Listening on port" << listenPort;

}

Listener::~Listener(){
    //exit and delete each thread
    foreach(Responder *responder, mRespondersList){
        responder->quit();
        responder->wait();
        delete responder;
    }
}

void Listener::connectionClosed(quintptr socketDescriptor){
    Responder *responder = mRespondersList.value(socketDescriptor);
    responder->wait();
    delete responder;
    qDebug() << "Connection Finished";
}
void Listener::closeConnection(QString name){
    //TODO: quand on auras la comparaison de clefs
}

void Listener::incomingConnection(qintptr socketDescriptor){
    Responder *responder = new Responder(socketDescriptor);
    responder->start();
    mRespondersList.insert(socketDescriptor, responder);
    connect(mRespondersList.value(socketDescriptor), SIGNAL(disconnected(quintptr)),
            this, SLOT(connectionClosed(quintptr)));
    qDebug() << "Starting Responder";
}
