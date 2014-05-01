#include "networkmanager.h"

//Starter
NetworkManager::NetworkManager(QTcpSocket *socket, ContactDB *responder_contactdb, QObject *parent): QObject(parent){

    m_Socket = socket;
    m_Contact = responder_contactdb;
    m_Root = new AbstractLink(m_Socket);


    qDebug()<<"NetworkManager::NetworkManager(socket)               Done";


}
//Responder
NetworkManager::NetworkManager(Contact *contact, ContactDB *starter_contactdb, QObject *parent): QObject(parent){

    m_Socket = new QTcpSocket;
    m_Root = new AbstractLink(contact);


    qDebug()<<"NetworkManager::NetworkManager(contact)              Done";
}

NetworkManager::~NetworkManager(){

    m_Socket->close();

    delete m_Socket, m_Root;


}
