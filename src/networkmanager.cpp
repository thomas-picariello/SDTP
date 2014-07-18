#include "networkmanager.h"

NetworkManager::NetworkManager(Contact *contact, QTcpSocket *socket, QByteArray gcmKey, QByteArray gcmBaseIV, QObject *parent):
    QObject(parent),
    m_contact(contact),
    m_gcmBaseIv(gcmBaseIV),
    m_gcmKey(gcmKey)
{
    dynamic_cast<TcpLink*>(getGcmDevice(TCP)->getLink())->setSocket(socket);
    getGcmDevice(TCP)->setKeyAndBaseIV(m_gcmKey, m_gcmBaseIv);
    connect(getGcmDevice(TCP), &QIODevice::readyRead,
            this, &NetworkManager::routeIncommingData);
    connect(&m_appManager, &AppManager::sendData,
            this, &NetworkManager::sendData);
    connect(&m_appManager, &AppManager::startAppFor,
            this, &NetworkManager::onStartAppForRequest);
}

QString NetworkManager::getHost(){
    return getGcmDevice(TCP)->getLink()->getHost();
}

Contact* NetworkManager::getContact() const{
    return m_contact;
}

int NetworkManager::getContactId() const{
    if(m_contact)
        return m_contact->getId();
    else
        return 0;
}

QString NetworkManager::getErrorString(Error err) const{
    QMetaEnum errorEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("Error"));
    return QString(errorEnum.valueToKey(static_cast<int>(err)));
}

void NetworkManager::registerApp(AppUID localUID, AbstractApp *app){
    connect(app, &AbstractApp::sendData,
            this, &NetworkManager::sendData);
    m_appManager.registerApp(localUID, app);
}

void NetworkManager::requestPartnerApp(AppUID localUID){
    m_appManager.requestPartnerApp(localUID);
}

void NetworkManager::registerAppConnection(AppUID localUID, AppUID distantUID){
    m_appManager.registerConnection(localUID, distantUID);
}

void NetworkManager::unregisterApp(AppUID uid){
    disconnect(m_appManager.getApp(uid), &AbstractApp::sendData,
            this, &NetworkManager::sendData);
    m_appManager.unregisterApp(uid);
}

void NetworkManager::onTcpDisconnect(){
    m_contact->setStatus(Contact::Offline);
    emit disconnected(m_contact);
}

void NetworkManager::sendData(LinkType linkType, QByteArray &data){
    AppManager* manager = dynamic_cast<AppManager*>(sender());
    AbstractApp* app = dynamic_cast<AbstractApp*>(sender());
    if(manager || app){
        if(manager || m_appManager.isAppRegistered(app)){
            Packet packet;
            if(manager)
                packet.destAppUID = AppUID(Manager);
            else
                packet.destAppUID = m_appManager.getDistantAppUID(app);
            packet.payload = data;

            if(packet.destAppUID.type() != AppType::Undefined){
                QByteArray packetBytes;
                QDataStream(&packetBytes, QIODevice::WriteOnly) << packet;
                getGcmDevice(TCP)->write(packetBytes);
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
            m_appManager.readIncommingData(packet.payload);
        else if(!m_appManager.isLocalAppConnected(packet.destAppUID))
            emit error(UnconnectedApp);
        else if(packet.payload.isEmpty())
            emit error(NoPayload);
        else{
            m_appManager.getApp(packet.destAppUID)->readIncommingData(packet.payload);
        }
    }
}

void NetworkManager::onStartAppForRequest(AppUID distantUID){
    if(m_contact)
        emit startAppFor(m_contact, distantUID);
}

GcmDevice* NetworkManager::getGcmDevice(LinkType linkType){
    GcmDevice* gcmDevice = NULL;
    if(m_gcmDevicesList.contains(linkType)){
        gcmDevice = m_gcmDevicesList.value(linkType);
    }else{
        switch(linkType){
        case TCP:
            gcmDevice = new GcmDevice(new TcpLink(), this);
            m_gcmDevicesList.insert(linkType, gcmDevice);
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
