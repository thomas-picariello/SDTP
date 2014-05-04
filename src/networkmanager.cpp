#include "networkmanager.h"

//Starter
NetworkManager::NetworkManager(QTcpSocket *socket, ContactDB *responder_contactdb, QObject *parent): QObject(parent){

    m_Socket = socket;
    m_Root = new AbstractLink(m_Socket);

    m_contact = NULL;

    agent = m_Root->getagent();



}
//Responder
NetworkManager::NetworkManager(Contact *contact, ContactDB *starter_contactdb, QObject *parent): QObject(parent){

    m_Socket = new QTcpSocket;
    m_Root = new AbstractLink(contact);

    connect(m_Root,SIGNAL(connected(ContactListWindow::Status)),this,SIGNAL(onStatusChanged(ContactListWindow::Status)));

    m_contact = contact;



    agent = m_Root->getagent();




}
void NetworkManager::chat(QByteArray data){


}
PacketAgent* NetworkManager::newAgent(){

    return NULL;
}
PacketAgent* NetworkManager::getRootAgent(){

    return m_Root->getagent();
}
Contact* NetworkManager::getcontactID(){
    return m_contact;
}

NetworkManager::~NetworkManager(){

    m_Socket->close();

    delete m_Socket, m_Root;


}
