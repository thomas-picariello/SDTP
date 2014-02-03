#include "handshake.h"

Handshake::Handshake(QTcpSocket *socket,  QObject *parent): QObject(parent)
{
    //responder

    m_Socket = socket;

    m_Settings = new QSettings;
    m_Settings->beginGroup("Settings");


     m_ResponderMsg = new QByteArray;


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
    else if(m_StarterMsg->data() == (QByteArray)"OKAY"){

        qDebug()<<"starter handshakesuccesfull";
        disconnect(m_Socket,SIGNAL(readyRead()),
                   this,SLOT(startCheckCompatibility()));
        emit handshakeSuccessfull();

    }
    else qDebug()<<"startcheckCompatibility ERROR.";




}
void Handshake::respondCheckKey(){

    m_ResponderMsg->append(m_Socket->readAll());
    qDebug()<<"message recieved"<<m_ResponderMsg->data();


    m_contact = ContactFactory::findByKey(m_ResponderMsg->data());

    //qDebug()<<"contactKey : "<<m_contact->getName()<<"Starter Key : "<<m_ResponderMsg->data();

    if(m_contact == NULL)
    {
        m_Socket->write("Bye");
        emit connectionClosed();
    }
    else
    {
        qDebug()<<"OKAY";
        m_Socket->write("OKAY");

        qDebug()<<"Responder handshake succesfull";
        disconnect(m_Socket,SIGNAL(readyRead()),
                   this,SLOT(respondCheckKey()));
        emit handshakeSuccessfull();
    }
}
void Handshake::respondCheckCompatbility(){

}

Handshake::~Handshake()
{

}
