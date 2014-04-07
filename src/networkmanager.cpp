#include "networkmanager.h"

NetworkManager::NetworkManager(QTcpSocket *socket, ContactDB *responder_contactdb, QObject *parent): QObject(parent){
    m_Socket = socket;
    m_contact = new Contact();
    m_TimeStamp = 0;
    m_PacketCounter = 0;

    //m_root = new AbstractLink();



    m_handshake = new Handshake(m_Socket,responder_contactdb);
    connect(m_handshake,SIGNAL(handshakeSuccessfull()),this,SLOT(onIdentified()));


}

NetworkManager::NetworkManager(Contact *contact, ContactDB *starter_contactdb, QObject *parent): QObject(parent){
    m_Socket = new QTcpSocket;

    m_TimeStamp = 0;
    m_PacketCounter = 0;
    m_contact = contact;

    QString ip  = m_contact->getHost();
    quint16 port = m_contact->getPort();



    //Close previous connection if already connected with this socket
    if(m_Socket->state() == QAbstractSocket::ConnectedState ||
       m_Socket->state() == QAbstractSocket::ConnectingState)
        m_Socket->abort();

    m_Socket->connectToHost(ip, port);

    connect(m_Socket, SIGNAL(connected()),
            this, SLOT(onConnect()));
}
void NetworkManager::onConnect(){

    m_dateTime = new QDateTime();

    m_handshake = new Handshake(m_Socket,starter_contactdb,m_contact);
    connect(m_handshake,SIGNAL(handshakeSuccessfull()),this,SLOT(onIdentified()));
    connect(m_handshake,SIGNAL(connectionClosed()),
            this,SLOT(deleteLater()));
    m_handshake->startCheckKey();

}

void NetworkManager::onIdentified(){


    m_contact = m_handshake->getContact();

    qDebug()<<m_contact->getName();

    QByteArray key;
    key = m_handshake->getkey();

    m_handshake = 0;
    delete m_handshake;

    m_settings = new QSettings;

    m_voip = new VoIP();

    mAesKey = key;


    mAesIv = QByteArray(AES::BLOCKSIZE, 0x0); //filled with 0x0.
    mCfbAesEnc.SetKeyWithIV((byte*)mAesKey.data(), mAesKey.size(), (byte*)mAesIv.data());
    mCfbAesDec.SetKeyWithIV((byte*)mAesKey.data(), mAesKey.size(), (byte*)mAesIv.data());

    m_MessengerWindow = new MessengerWindow((qint8)MESSENGER);
     QString ip = m_Socket->peerAddress().toString();

     m_MessengerWindow->displayMessage(Message("Connected to : "+m_contact->getName()+"("+ip+")", Message::SERVICE));




     connect(m_voip,SIGNAL(readyRead()),
             this, SLOT(onVoIPReadyRead()));
     connect(m_MessengerWindow,SIGNAL(callContact()),
             this, SLOT(voipCall()));
     connect(m_Socket, SIGNAL(readyRead()),
             this, SLOT(readIncomingData()));
     connect(m_MessengerWindow,SIGNAL(sendMessage(QByteArray,quint8)),
                     this, SLOT(sendData(QByteArray,quint8)));
}
void NetworkManager::voipCall(){

        if(m_voip->openMode() != VoIP::ReadWrite){
            m_MessengerWindow->changeButtonState(false);
            m_voip->start();
            sendData("1",SYSTEM);
        }
        else{
            m_MessengerWindow->changeButtonState(true);
            m_voip->stop();
            sendData("0",SYSTEM);
        }

}

void NetworkManager::onVoIPReadyRead()
{
    //qDebug()<<"new data from m_voip";
    sendData(m_voip->readAll(),VOIP);

}
void NetworkManager::readIncomingData(){
    QByteArray data = m_Socket->readAll();
    //decrypt AES
    mCfbAesDec.ProcessString((byte*)data.data(), data.length());

    quint16 TimeStampParse;
    TimeStampParse = qFromBigEndian<quint16>((uchar*)data.mid(0,2).data());


    quint8 PacketCounterParse = data.at(2);



    quint8 appIDparse = data.at(3);

    data = data.remove(0,4);

    if(TimeStampParse <= m_dateTime->currentMSecsSinceEpoch()%64536+2000 )
    {
        //qDebug()<<"TimeStamp : "<<TimeStampParse<<" CurrentTime : "<<m_dateTime->currentMSecsSinceEpoch()%64536<<"  Difference : "<<(m_dateTime->currentMSecsSinceEpoch()%64536)-TimeStampParse;
            if (appIDparse == MESSENGER)
            {
                m_MessengerWindow->displayMessage(Message(QString(data), Message::RECIEVED));
            }
            else if (appIDparse == VOIP){
                m_voip->write(data);
            }
            else if (appIDparse == SYSTEM){// 0 = VOIPOFF 1 = VOIPON
                m_MessengerWindow->displayMessage(Message(QString("SYSTEM message : "+data), Message::SERVICE));
                if(data == "1"){
                    if(m_voip->openMode() != VoIP::ReadWrite){
                        m_MessengerWindow->changeButtonState(false);
                        m_voip->start();
                    }
                    else sendData("1",SYSTEM);
                }
                else if (data == "0"){
                    m_MessengerWindow->changeButtonState(true);
                    m_voip->stop();
                }
            }
            else m_MessengerWindow->displayMessage(Message(QString("An unknown app has sent the following message : \n"+data), Message::ERR));
    }
    else qDebug()<<"TimeStamp : "<<TimeStampParse<<" CurrentTime : "<<m_dateTime->currentMSecsSinceEpoch()%64536<<"  Difference : "<<(m_dateTime->currentMSecsSinceEpoch()%64536)-TimeStampParse;



}
void NetworkManager::sendData(QByteArray data, quint8 appID){

    if(m_Socket->state() == QAbstractSocket::ConnectedState){

        if (appID == MESSENGER) m_MessengerWindow->displayMessage(Message(QString(data), Message::SENT));
        else if (appID == SYSTEM) m_MessengerWindow->displayMessage(Message(QString("SYSTEM message : "+data), Message::SERVICE));
        else if (appID == VOIP);
        else m_MessengerWindow->displayMessage(Message(QString("An unknown app has sent the following message : \n"+data), Message::ERR));
        m_TimeStamp = m_dateTime->currentMSecsSinceEpoch()%64536;

        m_PacketCounter++;

        data.prepend(appID);
        data.prepend(m_PacketCounter);
        data.prepend(m_TimeStamp&0xff);
        data.prepend((m_TimeStamp>>8)&0xff);


        //encrypt AES


        mCfbAesEnc.ProcessString((byte*)data.data(), data.length());
        m_Socket->write(data);



    }else{
        m_MessengerWindow->displayMessage(Message("Couldn't send message : Not connected.", Message::ERR));
    }
}

void NetworkManager::error(QAbstractSocket::SocketError error){
    qDebug()<<error;
    if(error == QAbstractSocket::RemoteHostClosedError)
        m_MessengerWindow->displayMessage(Message("The server has disconnected...", Message::ERR));
}

NetworkManager::~NetworkManager(){

    m_Socket->close();

    delete m_Socket,m_settings,m_voip,m_handshake,m_contact,m_MessengerWindow,mAesIv
            ,mAesKey,mCfbAesDec,mCfbAesEnc;


}
