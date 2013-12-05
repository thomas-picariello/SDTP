#include "starter.h"

Starter::Starter(QObject *parent) : QObject(parent){
    mMessenger = new Messenger;


    mSocket = new QTcpSocket;

    connect(mSocket, SIGNAL(readyRead()),
            this, SLOT(onDataRecieved()));

    connect(mSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(onError(QAbstractSocket::SocketError)));

    connect(mSocket, SIGNAL(connected()),
            this, SLOT(onConnect()));

    connect(mMessenger, SIGNAL(sendMessage(QByteArray)),
            this, SLOT(onSendData(QByteArray)));

}


void Starter::onConnect(){
    QString ip = mSocket->peerAddress().toString();
    mMessenger->displayMessage("Connected to: " + ip);
    qDebug() << "Connected to:" << ip;
    mSocket->write("Hello from client");
}
void Starter::onDataRecieved(){

    qDebug() << "Data recieved...";
    QByteArray data = mSocket->readAll();
    mMessenger->displayMessage(QString(data));
    qDebug() << "Data recieved:" << data;
}
void Starter::onError(QAbstractSocket::SocketError error) {
    qDebug() << error;
}
void Starter::onSendData(QByteArray data){
    qDebug() << "Send:" << data;
    mSocket->write(data);
    qDebug() << "Sent:" << data;


}

void Starter::openConnection(QString name){
    QSettings settings;

    QString ip  = settings.value("Contacts/"+name+"/IP").toString();

    //quint16 port = settings.value("Contacts/"+name+"/port").toInt();
    quint16 port = settings.value("Settings/port").toInt();

    QByteArray key = settings.value("Contacts/"+name+"/key").toString().toUtf8();

    //Close previous connection if already connected with this socket
    if(mSocket->state() == QAbstractSocket::ConnectedState ||
       mSocket->state() == QAbstractSocket::ConnectingState)
    {
        qDebug() << "Closing previously open(ing) connection";
        mSocket->abort();
    }

    qDebug() << "Opening connection with:" << ip << "...";
    mSocket->connectToHost(ip, port);
}

Starter::~Starter()
{
        mSocket->close();
        delete mSocket, mMessenger;
}
