#include "starter.h"

Starter::Starter(QObject *parent) : QObject(parent){
    mMessengerWindow = new MessengerWindow;

    mSocket = new QTcpSocket;

    //Setup crypto
    mAesKey = QByteArray(16, 0x0); //16 byte = 128 bits, filled with 0x0.
    mAesIv = QByteArray(AES::BLOCKSIZE, 0x0); //filled with 0x0.
    mCfbAesEnc.SetKeyWithIV((byte*)mAesKey.data(), mAesKey.size(), (byte*)mAesIv.data());
    mCfbAesDec.SetKeyWithIV((byte*)mAesKey.data(), mAesKey.size(), (byte*)mAesIv.data());

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
    mMessengerWindow->displayMessage(Message("Connected to: " + ip, Message::SERVICE));
    //qDebug() << "Connected to:" << ip;
    //mSocket->write("Starter connected");
}
void Starter::onDataRecieved(){
    //qDebug() << "Data recieved...";
    QByteArray data = mSocket->readAll();
    //decrypt AES
    mCfbAesDec.ProcessString((byte*)data.data(), data.length());
    mMessengerWindow->displayMessage(Message(QString(data), Message::RECIEVED));
    //qDebug() << "Data recieved:" << data;
}
void Starter::onError(QAbstractSocket::SocketError error){
    qDebug() << error;

    if(error = QAbstractSocket::RemoteHostClosedError){
        mMessengerWindow->displayMessage(Message("The server has disconnected...", Message::ERR));
    }
}
void Starter::onSendData(QByteArray data){
    //qDebug() << "Send:" << data;
    if(mSocket->state() == QAbstractSocket::ConnectedState){   
        mMessengerWindow->displayMessage(Message(data, Message::SENT));
        //encrypt AES
        mCfbAesEnc.ProcessString((byte*)data.data(), data.length());
        mSocket->write(data);
    }
    else mMessengerWindow->displayMessage(Message("Couldn't send message : Not connected.", Message::ERR));

    //qDebug() << "Sent:" << data;
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

Starter::~Starter(){
        mSocket->close();
        delete mSocket, mMessengerWindow;
}
