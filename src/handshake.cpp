#include "handshake.h"

Handshake::Handshake(QTcpSocket *socket,  QObject *parent): QObject(parent)
{
    //responder

   //m_Socket = new QTcpSocket;
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
    //m_Socket = new QTcpSocket;
    m_Socket = socket;

    m_Settings = new QSettings;
    m_Settings->beginGroup("Settings");

    m_contact = new Contact();
    m_contact = contact;


    m_StarterMsg = new QByteArray;

}
void Handshake::startCheckKey(){


    m_Socket->write(m_Settings->value("key").toByteArray());
    qDebug()<<"send : "<<m_Settings->value("key").toByteArray();


    qDebug()<<"starter handshake succesfull";
    emit handshakeSuccessfull();

}
void Handshake::startCheckCompatibility(){

}
void Handshake::respondCheckKey(){

    qDebug()<<"message recieved"<<m_Socket->readAll();


    qDebug()<<"Responder handshake succesfull";
    disconnect(m_Socket,SIGNAL(readyRead()),
               this,SLOT(respondCheckKey()));
    emit handshakeSuccessfull();

}
void Handshake::respondCheckCompatbility(){

}

Handshake::~Handshake()
{

}
