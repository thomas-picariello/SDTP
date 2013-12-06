#include "listener.h"

Listener::Listener(quint16 listenPort, QObject *parent) : QTcpServer(parent){
    listen(QHostAddress::Any, listenPort);
    qDebug() << "Listening on port" << listenPort;

}

void Listener::incomingConnection(qintptr socketDescriptor){



    qDebug() << "Starting Responder";



}
