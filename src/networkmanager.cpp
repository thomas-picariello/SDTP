#include "networkmanager.h"

NetworkManager::NetworkManager(QTcpSocket *socket, QObject *parent): QObject(parent)
{
    m_Socket = new QTcpSocket;
    m_Socket = socket;

    mAesKey = QByteArray(16, 0x0); //16 byte = 128 bits, filled with 0x0.
    mAesIv = QByteArray(AES::BLOCKSIZE, 0x0); //filled with 0x0.
    mCfbAesEnc.SetKeyWithIV((byte*)mAesKey.data(), mAesKey.size(), (byte*)mAesIv.data());
    mCfbAesDec.SetKeyWithIV((byte*)mAesKey.data(), mAesKey.size(), (byte*)mAesIv.data());


    m_MessengerWindow = new MessengerWindow;

    QString ip = m_Socket->peerAddress().toString();
    qDebug()<<ip;

    voip = new VoIP;

    m_MessengerWindow->displayMessage(Message("Connected to : "+ip, Message::SERVICE));

    connect(m_MessengerWindow,SIGNAL(callContact()),
            this, SLOT(call()));
    connect(m_Socket, SIGNAL(readyRead()),
            this, SLOT(readIncomingData()));
    connect(m_MessengerWindow,SIGNAL(sendMessage(QByteArray)),
                    this, SLOT(sendData(QByteArray)));



}
NetworkManager::NetworkManager(QString name, QObject *parent)
{
    Contact contact(name);
    m_Socket = new QTcpSocket;

    QString ip  = contact.getIp();

    //quint16 port = settings.value("Contacts/"+name+"/port").toInt();
    quint16 port = contact.getPort().toInt();

    QByteArray key = contact.getKey();

    //Close previous connection if already connected with this socket
    if(m_Socket->state() == QAbstractSocket::ConnectedState ||
       m_Socket->state() == QAbstractSocket::ConnectingState)
    {
        qDebug() << "Closing previously open(ing) connection";
        m_Socket->abort();
    }

    qDebug() << "Opening connection with:" << ip << "...";
    m_Socket->connectToHost(ip, port);




    mAesKey = QByteArray(16, 0x0); //16 byte = 128 bits, filled with 0x0.
    mAesIv = QByteArray(AES::BLOCKSIZE, 0x0); //filled with 0x0.
    mCfbAesEnc.SetKeyWithIV((byte*)mAesKey.data(), mAesKey.size(), (byte*)mAesIv.data());
    mCfbAesDec.SetKeyWithIV((byte*)mAesKey.data(), mAesKey.size(), (byte*)mAesIv.data());


    m_MessengerWindow = new MessengerWindow;

    voip = new VoIP;
    connect(m_MessengerWindow,SIGNAL(callContact()),
            this, SLOT(call()));
    connect(m_Socket, SIGNAL(readyRead()),
            this, SLOT(readIncomingData()));
    connect(m_MessengerWindow,SIGNAL(sendMessage(QByteArray)),
            this, SLOT(sendData(QByteArray)));
    connect(m_Socket, SIGNAL(connected()),
            this, SLOT(onConnect()));

}
void NetworkManager::onConnect(){
    QString ip = m_Socket->peerAddress().toString();
    m_MessengerWindow->displayMessage(Message("Connected to: " + ip, Message::SERVICE));
    qDebug() << "Connected to:" << ip;
    m_Socket->write("Starter connected");




}
void NetworkManager::voipCall()
{

    voip->call(Contact());
}

void NetworkManager::readIncomingData()
{
    QByteArray data = m_Socket->readAll();
    //decrypt AES
    mCfbAesDec.ProcessString((byte*)data.data(), data.length());

    m_MessengerWindow->displayMessage(Message(QString(data), Message::RECIEVED));


}

void NetworkManager::sendData(QByteArray data)
{
    if(m_Socket->state() == QAbstractSocket::ConnectedState){
        m_MessengerWindow->displayMessage(Message(QString(data), Message::SENT));
        //encrypt AES
        mCfbAesEnc.ProcessString((byte*)data.data(), data.length());

        m_Socket->write(data);
    }
    else m_MessengerWindow->displayMessage(Message("Couldn't send message : Not connected.", Message::ERR));

}
void NetworkManager::error(QAbstractSocket::SocketError error)
{
    qDebug()<<error;

    if(error == QAbstractSocket::RemoteHostClosedError){
        m_MessengerWindow->displayMessage(Message("The server has disconnected...", Message::ERR));
    }
}

NetworkManager::~NetworkManager()
{


}
