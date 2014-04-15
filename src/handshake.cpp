#include "handshake.h"

Handshake::Handshake(QTcpSocket *socket,ContactDB *contactdb,  QObject *parent): QObject(parent)
{
    //responder

    m_contactdb = contactdb;
    m_key = new QByteArray;

    m_Socket = socket;

    m_Settings = new QSettings("settings.ini", QSettings::IniFormat);




     m_CompatibleVersions = new QStringList();

     m_CompatibleVersions->append(QCoreApplication::applicationVersion());
     m_CompatibleVersions->append("BetaTestingVersion");


     qDebug()<<"responder constructor";
    connect(m_Socket,SIGNAL(readyRead()),
            this,SLOT(respondCheckKey()));

}

Handshake::Handshake(QTcpSocket *socket,ContactDB *contactdb,Contact *contact,  QObject *parent): QObject(parent)
{
    //starter

    m_contactdb = contactdb;
    m_key = new QByteArray;
    m_Socket = socket;

    m_Settings = new QSettings("settings.ini", QSettings::IniFormat);



    m_CompatibleVersions = new QStringList();

    m_CompatibleVersions->append(QCoreApplication::applicationVersion());
    m_CompatibleVersions->append("BetaTestingVersion");


    m_contact = contact;


}
void Handshake::startCheckKey(){





    RsaKeyring keyring(new QPair<QByteArray,QByteArray>());


    m_Socket->write(keyring.getPublicKey());

    connect(m_Socket,SIGNAL(readyRead()),
            this,SLOT(startCheckCompatibility()));




}
void Handshake::startCheckCompatibility(){

    QByteArray Msg;
    Msg.append(m_Socket->readAll());

    if(Msg.data() == (QByteArray)"Bye"){
        qDebug()<<"Bad Key";
        emit connectionClosed();
    }
    else if(Msg.data() == (QByteArray)"1"){

        qDebug()<<"starter handshakesuccesfull";
        disconnect(m_Socket,SIGNAL(readyRead()),
                   this,SLOT(startCheckCompatibility()));
        connect(m_Socket,SIGNAL(readyRead()),
                this,SLOT(finishHandshake()));
        m_Socket->write(QCoreApplication::applicationVersion().toUtf8());


    }
    else qDebug()<<"startcheckCompatibility ERROR.";




}
void Handshake::respondCheckKey(){

    QByteArray msg;
    msg.append(m_Socket->readAll());
    qDebug()<<"message recieved"<<msg.data();


    //m_contact = ContactFactory::findByKey(msg.data());
    m_contact = m_contactdb->findByKey(msg.data());

    if(m_contact == NULL)
    {
        m_Socket->write("Bye");
        emit connectionClosed();
    }
    else
    {
        qDebug()<<"OKAY";
        m_Socket->write("1");

        qDebug()<<"Responder handshake succesfull";
        disconnect(m_Socket,SIGNAL(readyRead()),
                   this,SLOT(respondCheckKey()));
        connect(m_Socket,SIGNAL(readyRead()),
                this,SLOT(respondCheckCompatbility()));
    }
}
void Handshake::respondCheckCompatbility(){

    disconnect(m_Socket,SIGNAL(readyRead()),
               this,SLOT(respondCheckCompatbility()));
    if(m_CompatibleVersions->contains(m_Socket->readAll()))
    {
        m_Socket->write("OKAY");
        emit handshakeSuccessfull();
    }
    else{

        m_Socket->write("ERROR");
        emit connectionClosed();
    }
}

void Handshake::finishHandshake()
{
    disconnect(m_Socket,SIGNAL(readyRead()),
               this,SLOT(finishHandshake()));
    if(m_Socket->readAll() == "OKAY"){
        emit handshakeSuccessfull();
    }
    else emit connectionClosed();

}

Contact* Handshake::getContact(){


    return m_contact;
}
QByteArray Handshake::getkey()
{

    QByteArray key = QByteArray(16, 0x0);



    if(key.length() != 16){
        qDebug()<<key.length();
        return QByteArray(16,0x0);
    }
    else return key;
}

Handshake::~Handshake()
{

}
