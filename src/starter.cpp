#include "starter.h"

Starter::Starter(QObject *parent) : QObject(parent){
    mMessengerWindow = new MessengerWindow;


    mSocket = new QTcpSocket;

    connect(mSocket, SIGNAL(readyRead()),
            this, SLOT(onDataRecieved()));

    connect(mSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(onError(QAbstractSocket::SocketError)));

    connect(mSocket, SIGNAL(connected()),
            this, SLOT(onConnect()));

    connect(mMessengerWindow, SIGNAL(sendMessage(QByteArray)),
            this, SLOT(onSendData(QByteArray)));

}


void Starter::onConnect(){
    QString ip = mSocket->peerAddress().toString();
    mMessengerWindow->displayMessage("Connected to: " + ip);
    qDebug() << "Connected to:" << ip;
    mSocket->write("Hello from client");
}
void Starter::onDataRecieved(){

    qDebug() << "Data recieved...";
    QByteArray data = mSocket->readAll();
    mMessengerWindow->displayMessage("Received : "+QString(data));
    qDebug() << "Data recieved:" << data;
}
void Starter::onError(QAbstractSocket::SocketError error) {
    qDebug() << error;

    if(error = QAbstractSocket::RemoteHostClosedError)
    {

        mMessengerWindow->displayMessage("The server has disconnected...");

    }


}
void Starter::onSendData(QByteArray data){
    qDebug() << "Send:" << data;
    if(mSocket->state() == QAbstractSocket::ConnectedState)
    {
        mSocket->write(data);
        mMessengerWindow->displayMessage("Sent : "+data);

    }
    else mMessengerWindow->displayMessage("!!!-"+data+"-!!!\n|Couldn't send message : Not connected.");

    qDebug() << "Sent:" << data;
}

void Starter::openConnection(QString name){
    Contact contact(name);

    QString ip  = contact.getIp();

    //quint16 port = settings.value("Contacts/"+name+"/port").toInt();
    quint16 port = contact.getPort().toInt();

    QByteArray key = contact.getKey();

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
        delete mSocket, mMessengerWindow;
}
