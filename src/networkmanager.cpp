#include "networkmanager.h"

//Starter
NetworkManager::NetworkManager(Contact *contact, ContactDB *contactDB, RsaKeyring *keyring, IpFilter *ipFilter, QObject *parent):
    QObject(parent),
    m_ContactDB(contactDB),
    m_Contact(contact),
    m_Status(Contact::Offline),
    m_GcmDecryptor(NULL),
    m_GcmEncryptor(NULL),
    m_LastTimeStamp(0),
    m_LastPacketNumber(0)
{
    TcpLink *tcpLink = dynamic_cast<TcpLink*>(getLink(TCP));
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
    m_GcmEncryptor(NULL),
    m_LastTimeStamp(0),
    m_LastPacketNumber(0)
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

void NetworkManager::onDisconnected(){
    m_Status = Contact::Offline;
    emit contactStatusChanged(getContactId(), m_Status);
}

void NetworkManager::onContactEvent(Contact::Event event){
    if(m_Contact){
        //TODO: do something intelligent with event
        //ie: if contact offline retry to connect after change other than name
        //m_Contact = m_ContactDB->findById(m_Contact->getId());
    }
}

void NetworkManager::sendData(LinkType linkType, QByteArray &data){
    AbstractApp *senderApp = dynamic_cast<AbstractApp*>(sender());
    if(senderApp){
        foreach(AbstractApp* registeredApp, m_AppList){
            if(senderApp == registeredApp){
                Packet packet;
                packet.timestamp;
                packet.packetNumber;
                packet.destAppUID = m_AppList.key(senderApp); //TODO: change for partner UID
                packet.payload = data;

                QByteArray serializedPacket;
                QDataStream(&serializedPacket, QIODevice::WriteOnly) << packet;
                QByteArray cypherPacket = gcmEncrypt(serializedPacket);
                getLink(linkType)->write(cypherPacket);
                return;
            }
        }
        emit error(UnregisteredApp);
    }
}

void NetworkManager::processIncommingData(){
    AbstractLink *link = dynamic_cast<AbstractLink*>(sender());
    if(link){
        QByteArray clearText = gcmDecrypt(link->readAll());
        QDataStream packetStream(&clearText, QIODevice::ReadOnly);
        Packet packet;
        packetStream >> packet;

        if(packet.timestamp < m_LastTimeStamp)
            emit error(BadTimestamp);
        else if((packet.packetNumber - m_LastPacketNumber) % sizeof(packet.packetNumber) != 1)
            emit error(BadPacketNumber);
        else if(m_AppList.find(packet.destAppUID) == m_AppList.end())
            emit error(AppNotStarted);
        else if(packet.payload.isEmpty())
            emit error(NoPayload);
        else
            m_AppList.value(packet.destAppUID)->readIncommingData(packet.payload);
    }
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
        connect(getLink(TCP), SIGNAL(readyRead()),
                this, SLOT(processIncommingData()));
        //TODO: start AppManager
    }else{
        if(m_Handshaker->getMode() == Handshaker::StarterMode)
            m_Pinger.start(m_Handshaker->getBanTime()); //restart pinger after ban time
        else if(m_Handshaker->getMode() == Handshaker::ResponderMode)
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

QByteArray NetworkManager::gcmDecrypt(QByteArray& cipherText){
    std::string clearText;
    try{
        CryptoPP::ArraySource((byte*)cipherText.data(), cipherText.size(), true,
                              new CryptoPP::AuthenticatedDecryptionFilter(*m_GcmDecryptor,
                                                                          new CryptoPP::StringSink(clearText)
                                                                          )
                              );
    }catch(CryptoPP::Exception&){
        emit error(PacketCorrupted);
    }
    return QByteArray(clearText.data(), (int)clearText.size());
}

QByteArray NetworkManager::gcmEncrypt(QByteArray& clearText){
    std::string cipherText;
    try{
        CryptoPP::ArraySource((byte*)clearText.data(), clearText.size(), true,
                              new CryptoPP::AuthenticatedEncryptionFilter(*m_GcmEncryptor,
                                                                          new CryptoPP::StringSink(cipherText)
                                                                          )
                              );
    }catch(CryptoPP::Exception&){
        emit error(BadSymmetricKey);
    }
    return QByteArray(cipherText.data(), (int)cipherText.size());
}

NetworkManager::~NetworkManager(){
    //Rem: do not delete m_ContactDB and m_AppList pointers
    delete m_Handshaker;
    if(m_GcmDecryptor)
        delete m_GcmDecryptor;
    if(m_GcmEncryptor)
        delete m_GcmEncryptor;
    emit destroyed(this);
}

QDataStream& operator <<(QDataStream &out, const NetworkManager::Packet &packet){
    out << packet.timestamp
        << packet.packetNumber
        << packet.destAppUID
        << packet.payload;
    return out;
}

QDataStream& operator >>(QDataStream &in, NetworkManager::Packet &packet){
    in >> packet.timestamp;
    in >> packet.packetNumber;
    in >> packet.destAppUID;
    in >> packet.payload;
    return in;
}
