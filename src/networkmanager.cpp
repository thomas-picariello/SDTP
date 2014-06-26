#include "networkmanager.h"

//Starter
NetworkManager::NetworkManager(Contact *contact, ContactDB *contactDB, RsaKeyring *keyring, IpFilter *ipFilter, QObject *parent):
    QObject(parent),
    m_ContactDB(contactDB),
    m_Contact(contact),
    m_Status(Contact::Offline),
    m_GcmDecryptor(NULL),
    m_GcmEncryptor(NULL)
{
    TcpLink *tcpLink = dynamic_cast<TcpLink*>(getLink(TCP));
    m_Contact->setParent(this); //take ownership of the contact
    m_Handshaker = new Handshaker(tcpLink, keyring, this);
    m_Handshaker->setIpFilter(ipFilter);
    m_Pinger.setContact(m_Contact);
    m_Pinger.setLink(tcpLink);
    m_Pinger.start();

    connect(&m_Pinger, SIGNAL(connected()),
            this, SLOT(doStarterHandshake()));
    connect(m_Handshaker, SIGNAL(handshakeFinished(bool)),
            this, SLOT(onHandshakeFinished(bool)));
    connect(m_Handshaker, SIGNAL(newContactId(int)),
            this, SIGNAL(newContactId(int)));
    connect(m_Handshaker, SIGNAL(error(Handshaker::Error)),
            this, SLOT(onHandshakeError(Handshaker::Error)));
}

//Responder
NetworkManager::NetworkManager(QTcpSocket *socket, ContactDB *contactDB, RsaKeyring *keyring, IpFilter *ipFilter, QObject *parent):
    QObject(parent),
    m_ContactDB(contactDB),
    m_Contact(NULL),
    m_Status(Contact::Offline),
    m_GcmDecryptor(NULL),
    m_GcmEncryptor(NULL)
{
    TcpLink *tcpLink = dynamic_cast<TcpLink*>(getLink(TCP));
    tcpLink->setSocket(socket); //give the socket to the TCP link
    m_Handshaker = new Handshaker(tcpLink, keyring, this);
    m_Handshaker->setIpFilter(ipFilter);

    connect(m_Handshaker, SIGNAL(handshakeFinished(bool)),
            this, SLOT(onHandshakeFinished(bool)));
    connect(m_Handshaker, SIGNAL(error(Handshaker::Error)),
            this, SLOT(onHandshakeError(Handshaker::Error)));

    waitForHandshake();
}

int NetworkManager::getContactId() const{
    if(m_Contact)
        return m_Contact->getId();
    else
        return 0;
}

QString NetworkManager::getErrorString(Error err) const{
    QMetaEnum errorEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("Error"));
    return QString(errorEnum.valueToKey(static_cast<int>(err)));
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
        //ie: if contact offline retry to connect after change other than name
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

void NetworkManager::waitForHandshake(){
    m_Handshaker->waitForHandshake(m_ContactDB);
}

void NetworkManager::doStarterHandshake(){
    m_Handshaker->beginStarterHandshake(m_Contact);
}

void NetworkManager::onHandshakeFinished(bool successfull){
    if(successfull){
        if(m_GcmDecryptor) delete m_GcmDecryptor;
        if(m_GcmEncryptor) delete m_GcmEncryptor;
        m_GcmDecryptor = m_Handshaker->getGcmDecryptor();
        m_GcmEncryptor = m_Handshaker->getGcmEncryptor();
        m_Contact = m_Handshaker->getContact();
        emit contactStatusChanged(getContactId(), Contact::Online);
    }else{
        if(m_Handshaker->getMode() == Handshaker::StarterMode)
            m_Pinger.start(m_Handshaker->getBanTime()); //restart pinger after ban time
        else if(m_Handshaker->getMode() == Handshaker::ResponderMode)
            //TODO: update ban time
            deleteLater(); //close connection, delete the network manager
    }
}

void NetworkManager::onHandshakeError(Handshaker::Error err){
     //TODO: remove
    qDebug()<<"Handshake error:"<<m_Handshaker->getErrorString(err);
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
    emit destroyed(this);
}
