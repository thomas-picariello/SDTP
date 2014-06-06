#include "networkmanager.h"

//Starter
NetworkManager::NetworkManager(Contact *contact, ContactDB *contactDB, QPair<QByteArray, QByteArray> *fileKey, QObject *parent):
    QObject(parent),
    m_ContactDB(contactDB),
    m_Contact(contact),
    m_Status(Contact::Offline)
{
    TcpLink *tcpLink = dynamic_cast<TcpLink*>(getLink(TCP));
    m_Contact->setParent(this); //take ownership of the contact
    m_Handshaker = new Handshaker(tcpLink, fileKey, this);
    m_Pinger.setContact(m_Contact);
    m_Pinger.setLink(tcpLink);
    m_Pinger.start();

    connectSignals();
}

//Responder
NetworkManager::NetworkManager(QTcpSocket *socket, ContactDB *contactDB, QPair<QByteArray, QByteArray> *fileKey, QObject *parent):
    QObject(parent),
    m_ContactDB(contactDB),
    m_Contact(NULL),
    m_Status(Contact::Offline)
{
    TcpLink *tcpLink = dynamic_cast<TcpLink*>(getLink(TCP));
    tcpLink->setSocket(socket); //give the socket to the TCP link
    m_Handshaker = new Handshaker(tcpLink, fileKey, this);
    doResponderHandshake(tcpLink);

    connectSignals();
}

void NetworkManager::connectSignals(){
    connect(&m_Pinger, SIGNAL(connected(QString)),
            this, SLOT(doStarterHandshake(QString)));
    connect(m_Handshaker, SIGNAL(handshakeFinished(bool)),
            this, SLOT(onHandshakeFinished(bool)));
}

int NetworkManager::getContactId() const{
    if(m_Contact)
        return m_Contact->getId();
    else
        return 0;
}

Contact* NetworkManager::getContact() const{
    return m_Contact;
}

void NetworkManager::registerApp(AbstractApp::AppUID uid, AbstractApp *app){
    if(!m_AppList.contains(uid))
        m_AppList.insert(uid, app);
}

void NetworkManager::unregisterApp(AbstractApp::AppUID uid){
    if(m_AppList.contains(uid))
        m_AppList.remove(uid);
}

void NetworkManager::onDisconnected(){
    m_Status = Contact::Offline;
    emit contactStatusChanged(getContactId(), m_Status);
}

void NetworkManager::onContactEvent(Contact::Event event){
    if(m_Contact){
        //TODO: do something intelligent with event
        m_Contact = m_ContactDB->findById(m_Contact->getId());
    }
}

void NetworkManager::sendData(QByteArray &data, LinkType linkType){
    //forge
    //encrypt
    //send
}

void NetworkManager::processIncommingData(){
    //parse
    //decrypt
    //emit
}

void NetworkManager::doResponderHandshake(TcpLink *link){
    qDebug() << "Responder hanshaking with:" << link->getHost().first;
    m_Handshaker->beginResponderHandshake(m_ContactDB);
}

void NetworkManager::doStarterHandshake(QString host){
    qDebug() << "Starter hanshaking with:" << host;
    m_Handshaker->beginStarterHandshake(m_Contact);
}

void NetworkManager::onHandshakeFinished(bool successfull){
    //if sucessfull, get crypto info
    if(successfull)
        emit contactStatusChanged(getContactId(), Contact::Online);
}

AbstractLink* NetworkManager::getLink(LinkType linkType){
    AbstractLink* link = NULL;
    if(m_LinkList.contains(linkType)){
        link = m_LinkList.value(linkType);
    }else{
        switch(linkType){
        case TCP:
            link = new TcpLink();
            m_LinkList.insert(linkType, link);
            connect(dynamic_cast<TcpLink*>(link), SIGNAL(disconnected()),
                    this, SLOT(onDisconnected()));
            break;
        case UDP:
            //TODO: impl
            break;
        case RTP:
            //TODO: impl
            break;
        }
    }
    return link;
}

void NetworkManager::cleanLinks(){

}

NetworkManager::Packet NetworkManager::createPacket(QByteArray &payload){
    return Packet();
}

NetworkManager::Packet NetworkManager::parsePacket(QByteArray &packet){
    return Packet();
}

QByteArray NetworkManager::serializePacket(Packet &packet){
    return QByteArray();
}


NetworkManager::~NetworkManager(){
    //Rem: do not delete m_ContactDB and m_AppList pointers
    delete m_Handshaker;
}
