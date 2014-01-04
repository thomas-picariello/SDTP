#include "responder.h"

Responder::Responder(QTcpSocket *socket, QObject *parent): QObject(parent){
    m_messenger_window = new MessengerWindow;
    m_responderSocket = new QTcpSocket;
    m_responderSocket = socket;

    //Setup crypto
    mAesKey = QByteArray(16, 0x0); //16 byte = 128 bits, filled with 0x0.
    mAesIv = QByteArray(AES::BLOCKSIZE, 0x0); //filled with 0x0.
    mCfbAesEnc.SetKeyWithIV((byte*)mAesKey.data(), mAesKey.size(), (byte*)mAesIv.data());
    mCfbAesDec.SetKeyWithIV((byte*)mAesKey.data(), mAesKey.size(), (byte*)mAesIv.data());

    //qDebug()<<"new responder created";

    connect(m_responderSocket,SIGNAL(readyRead()),this,SLOT(readIncomingData()));
    connect(m_messenger_window,SIGNAL(sendMessage(QByteArray)),this,SLOT(sendData(QByteArray)));
    connect(m_responderSocket,SIGNAL(error(QAbstractSocket::SocketError)), this,SLOT(error(QAbstractSocket::SocketError)));
}

void Responder::readIncomingData(){
    QByteArray data = m_responderSocket->readAll();
    //decrypt AES
    mCfbAesDec.ProcessString((byte*)data.data(), data.length());

    m_messenger_window->displayMessage(Message(QString(data), Message::RECIEVED));

    //qDebug() << "incommingData !" << data;
}
void Responder::sendData(QByteArray data){
    if(m_responderSocket->state() == QAbstractSocket::ConnectedState){
        m_messenger_window->displayMessage(Message(QString(data), Message::SENT));
        //encrypt AES
        mCfbAesEnc.ProcessString((byte*)data.data(), data.length());

        m_responderSocket->write(data);
    }
    else m_messenger_window->displayMessage(Message("Couldn't send message : Not connected.", Message::ERR));
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
