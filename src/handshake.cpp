#include "handshake.h"

Handshake::Handshake(QTcpSocket *socket,  QObject *parent): QObject(parent)
{
    //responder

    m_Socket = socket;

    m_Settings = new QSettings;
    m_Settings->beginGroup("Settings");


     m_ResponderMsg = new QByteArray;

     m_CompatibleVersions = new QStringList();

     m_CompatibleVersions->append(QCoreApplication::applicationVersion());
     m_CompatibleVersions->append("BetaTestingVersion");


     qDebug()<<"responder constructor";
    connect(m_Socket,SIGNAL(readyRead()),
            this,SLOT(respondCheckKey()));

}

Handshake::Handshake(QTcpSocket *socket,Contact *contact,  QObject *parent): QObject(parent)
{
    //starter
    m_Socket = socket;

    m_Settings = new QSettings;
    m_Settings->beginGroup("Settings");

    m_StarterMsg = new QByteArray;

    m_CompatibleVersions = new QStringList();

    m_CompatibleVersions->append(QCoreApplication::applicationVersion());
    m_CompatibleVersions->append("BetaTestingVersion");


    m_contact = contact;


}
void Handshake::startCheckKey(){


    m_Socket->write(m_Settings->value("key").toByteArray());
    qDebug()<<"send : "<<m_Settings->value("key").toByteArray();

    connect(m_Socket,SIGNAL(readyRead()),
            this,SLOT(startCheckCompatibility()));




}
void Handshake::startCheckCompatibility(){

    m_StarterMsg->append(m_Socket->readAll());

    if(m_StarterMsg->data() == (QByteArray)"Bye"){
        qDebug()<<"Bad Key";
        emit connectionClosed();
    }
    else if(m_StarterMsg->data() == (QByteArray)"1"){

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

    m_ResponderMsg->append(m_Socket->readAll());
    qDebug()<<"message recieved"<<m_ResponderMsg->data();


    m_contact = ContactFactory::findByKey(m_ResponderMsg->data());

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


Handshake::~Handshake()
{

}
