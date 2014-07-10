#include "networkmanager.h"

NetworkManager::NetworkManager(Contact *contact, QTcpSocket *socket, QByteArray gcmKey, QByteArray gcmBaseIV, QObject *parent):
    QObject(parent),
    m_Contact(contact),
    m_GcmBaseIv(gcmBaseIV),
    m_GcmKey(gcmKey),
    m_Status(Contact::Offline)
{
    dynamic_cast<TcpLink*>(getGcmDevice(TCP)->getLink())->setSocket(socket);
    connect(getGcmDevice(TCP), &QIODevice::readyRead,
            this, &NetworkManager::routeIncommingData);
    connect(&m_AppManager, &AppManager::sendData,
            this, &NetworkManager::sendData);
    connect(&m_AppManager, &AppManager::startApp,
            this, &NetworkManager::onStartAppRequest);
    connect(&m_AppManager, &AppManager::startAppFor,
            this, &NetworkManager::onStartAppForRequest);
}

QString NetworkManager::getHost(){
    return getGcmDevice(TCP)->getLink()->getHost();
}

Contact* NetworkManager::getContact() const{
    return m_Contact;
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

//NetworkManager::State NetworkManager::getState() const{
//    return m_State;
//}

void NetworkManager::registerApp(AppUID localUID, AbstractApp *app){
    connect(app, &AbstractApp::sendData,
            this, &NetworkManager::sendData);
    m_AppManager.registerApp(localUID, app);
}

void NetworkManager::registerAppConnection(AppUID localUID, AppUID distantUID){
    m_AppManager.registerConnection(localUID, distantUID);
}

void NetworkManager::unregisterApp(AppUID uid){
    disconnect(m_AppManager.getApp(uid), &AbstractApp::sendData,
            this, &NetworkManager::sendData);
    m_AppManager.unregisterApp(uid);
}

void NetworkManager::onTcpDisconnect(){
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
    AppManager* manager = dynamic_cast<AppManager*>(sender());
    AbstractApp* app = dynamic_cast<AbstractApp*>(sender());
    if(manager || app){
        if(manager || m_AppManager.isAppRegistered(app)){
            Packet packet;
            if(manager)
                packet.destAppUID = AppUID(Manager);
            else
                packet.destAppUID = m_AppManager.getDistantAppUID(app);
            packet.payload = data;

            if(packet.destAppUID.type() != AppType::Undefined){
                QByteArray serializedPacket;
                QDataStream(&serializedPacket, QIODevice::WriteOnly) << packet;
                getGcmDevice(linkType)->write(serializedPacket);
            }else
                emit error(UnconnectedApp);
        }else
            emit error(UnregisteredApp);
    }
}

void NetworkManager::routeIncommingData(){
    GcmDevice *gcmDevice = dynamic_cast<GcmDevice*>(sender());
    if(gcmDevice){
        Packet packet;
        QDataStream (gcmDevice) >> packet;
        if(packet.destAppUID.type() == Manager)
            m_AppManager.readIncommingData(packet.payload);
        else if(m_AppManager.isAppConnected(packet.destAppUID))
            emit error(UnconnectedApp);
        else if(packet.payload.isEmpty())
            emit error(NoPayload);
        else{
            if(m_AppManager.isAppConnected(packet.destAppUID))
                m_AppManager.getApp(packet.destAppUID)->readIncommingData(packet.payload);
        }
    }
}

//void NetworkManager::waitForHandshake(){
//    m_Handshaker->waitForHandshake(m_ContactDB);
//}

//void NetworkManager::doStarterHandshake(){
//    m_Handshaker->beginStarterHandshake(m_Contact);
//}

//void NetworkManager::onHandshakeFinished(bool successfull){
//    if(successfull){
//        m_State = Connected;
//        m_GcmKey = m_Handshaker->getGcmKey();
//        m_GcmBaseIv = m_Handshaker->getGcmBaseIV();
//        getGcmDevice(TCP)->setKeyAndBaseIV(m_GcmKey, m_GcmBaseIv);
//        getGcmDevice(TCP)->setBypassMode(false);
//        m_Contact = m_Handshaker->getContact();
//        emit contactStatusChanged(getContactId(), Contact::Online); //TODO: Contact self-signal on internal status change
//        connect(getGcmDevice(TCP), &QIODevice::readyRead,
//                this, &NetworkManager::routeIncommingData);
//        connect(&m_AppManager, &AppManager::sendData,
//                this, &NetworkManager::sendData);
//        connect(&m_AppManager, &AppManager::startApp,
//                this, &NetworkManager::onStartAppRequest);
//        connect(&m_AppManager, &AppManager::startAppFor,
//                this, &NetworkManager::onStartAppForRequest);

//    }else{
//        if(m_Handshaker->getMode() == Handshaker::StarterMode){
//            disconnect(getGcmDevice(TCP), &QIODevice::readyRead,
//                    this, &NetworkManager::routeIncommingData);
//            disconnect(&m_AppManager, &AppManager::sendData,
//                    this, &NetworkManager::sendData);
//            disconnect(&m_AppManager, &AppManager::startApp,
//                    this, &NetworkManager::onStartAppRequest);
//            disconnect(&m_AppManager, &AppManager::startAppFor,
//                    this, &NetworkManager::onStartAppForRequest);
//            m_GcmKey.clear();
//            m_GcmBaseIv.clear();
////            m_State = Pinging;
////            m_Pinger.start(m_Handshaker->getRecievedBanTime()); //restart pinger after ban time
//        }else if(m_Handshaker->getMode() == Handshaker::ResponderMode)
//            deleteLater(); //close connection, delete the network manager
//    }
//}

void NetworkManager::onStartAppRequest(AppType type){
    if(m_Contact)
        emit startApp(m_Contact, type);
}

void NetworkManager::onStartAppForRequest(AppUID distantUID){
    if(m_Contact)
        emit startAppFor(m_Contact, distantUID);
}

void NetworkManager::onRouteReady(AbstractApp *app){
    //connect app signals
}

//void NetworkManager::onHandshakeError(Handshaker::Error err){
//     //TODO: report error in a proper way
//    qDebug()<<"Handshake error:"<<m_Handshaker->getErrorString(err);
//}

GcmDevice* NetworkManager::getGcmDevice(LinkType linkType){
    GcmDevice* gcmDevice = NULL;
    if(m_GcmDevicesList.contains(linkType)){
        gcmDevice = m_GcmDevicesList.value(linkType);
    }else{
        switch(linkType){
        case TCP:
            gcmDevice = new GcmDevice(new TcpLink(), this);
            m_GcmDevicesList.insert(linkType, gcmDevice);
            connect(dynamic_cast<TcpLink*>(gcmDevice->getLink()), &TcpLink::disconnected,
                    this, &NetworkManager::onTcpDisconnect);
            break;
        case UDP:
            //TODO: impl
            break;
        case RTP:
            //TODO: impl
            break;
        }
    }
    return gcmDevice;
}

void NetworkManager::cleanLinks(){

}

NetworkManager::~NetworkManager(){
    //Rem: do not delete m_ContactDB and m_AppList pointers
//    delete m_Handshaker;
    emit destroyed(this);
}

QDataStream& operator <<(QDataStream &out, const NetworkManager::Packet &packet){
    out << packet.destAppUID
        << packet.payload;
    return out;
}

QDataStream& operator >>(QDataStream &in, NetworkManager::Packet &packet){
    in >> packet.destAppUID;
    in >> packet.payload;
    return in;
}
