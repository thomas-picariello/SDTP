#include "networkmanager.h"

//Starter
NetworkManager::NetworkManager(QTcpSocket *socket, ContactDB *responder_contactdb, QObject *parent): QObject(parent){

    m_Socket = socket;
    m_Root = new AbstractLink(m_Socket);

    m_contact = NULL;
    m_ContactDB = responder_contactdb;

    agent = m_Root->getagent();



}
//Responder
NetworkManager::NetworkManager(Contact *contact, ContactDB *starter_contactdb, QObject *parent): QObject(parent){

    m_Socket = new QTcpSocket;
    qDebug()<<"Attempt to connect to "<<contact->getName();
    m_Root = new AbstractLink(contact);


    connect(m_Root,SIGNAL(connected(ContactListWindow::Status)),
            this,SLOT(onStatusChanged(ContactListWindow::Status)));

    m_contact = contact;
    m_ContactDB = starter_contactdb;



    agent = m_Root->getagent();




}
void NetworkManager::chat(QByteArray data){


}


void NetworkManager::onStatusChanged(ContactListWindow::Status status){
    emit statusChanged(getContactId(), status);
}
void NetworkManager::onContactEvent(Contact::Event event){
    if(m_contact){
        m_contact = m_ContactDB->findById(m_contact->getId());
    }
}

PacketAgent* NetworkManager::newAgent(){

    return NULL;
}
PacketAgent* NetworkManager::getRootAgent(){

    return m_Root->getagent();
}
int NetworkManager::getContactId(){
    if(m_contact)
        return m_contact->getId();
    else
        return 0;
}

NetworkManager::~NetworkManager(){


    delete m_Root;

}
