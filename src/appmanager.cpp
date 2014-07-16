#include "appmanager.h"

void AppManager::readIncommingData(QByteArray &data){
    Packet packet;
    QDataStream (&data, QIODevice::ReadOnly) >> packet;
    switch(packet.command){
    case StartAppCommand:
        emit startAppFor(packet.distantUID);
        break;
    case AppStartedSignal:
        registerConnection(packet.localUID, packet.distantUID);
    case AppClosedSignal:
        emit distantAppUnregistered(getLocalAppUID(packet.distantUID));
        unregisterApp(packet.distantUID);
        break;
    default:
        emit error(UnknownCommand);
    }
}

AbstractApp* AppManager::getApp(AppUID localUID) const{
    return m_LocalAppsRegister.value(localUID, NULL);
}

AppUID AppManager::getLocalAppUID(AbstractApp* app) const{
    return m_LocalAppsRegister.key(app);
}

AppUID AppManager::getLocalAppUID(AppUID distantAppUID) const{
    return m_AppConnectionsTable.key(distantAppUID);
}

AppUID AppManager::getDistantAppUID(AppUID localAppUID) const{
    return m_AppConnectionsTable.value(localAppUID);
}

AppUID AppManager::getDistantAppUID(AbstractApp* app) const{
    return getDistantAppUID(getLocalAppUID(app));
}

bool AppManager::isAppRegistered(AbstractApp* app) const{
    return (m_LocalAppsRegister.key(app).type() != Undefined);
}

bool AppManager::isAppConnected(AppUID appUID) const{
    return (m_AppConnectionsTable.key(appUID).type() != Undefined ||
            m_AppConnectionsTable.value(appUID).type() != Undefined );
}

void AppManager::requestPartnerApp(AppUID localAppUID){
    sendPacket(StartAppCommand, localAppUID, AppUID());
}

void AppManager::registerApp(AppUID localAppUID, AbstractApp *app){
    m_LocalAppsRegister.insert(localAppUID, app);
}

void AppManager::unregisterApp(AppUID localAppUID){
    m_AppConnectionsTable.remove(localAppUID);
    m_LocalAppsRegister.remove(localAppUID);
}

bool AppManager::registerConnection(AppUID localAppUID, AppUID distantAppUID){
    bool appRegistered = m_LocalAppsRegister.value(localAppUID, NULL) != NULL;
    if(appRegistered)
        m_AppConnectionsTable.insert(localAppUID, distantAppUID);
    return appRegistered;
}

void AppManager::sendAppStartedSignal(AppUID localUID){
    sendPacket(AppStartedSignal, localUID, getDistantAppUID(localUID));
}

void AppManager::sendPacket(Command cmd, AppUID localUID, AppUID distantUID){
    Packet packet(cmd, localUID, distantUID);
    QByteArray data;
    QDataStream(&data, QIODevice::WriteOnly) << packet;
    emit sendData(TCP, data);
}

QDataStream& operator<<(QDataStream &out, const AppManager::Packet &packet){
    out << (quint8)packet.command;
    out << packet.localUID;
    out << packet.distantUID;
    return out;
}

QDataStream& operator>>(QDataStream &in, AppManager::Packet &packet){
    quint8 cmd;
    in >> cmd;
    packet.command = static_cast<AppManager::Command>(cmd);
    in >> packet.distantUID;   //distant and local UID inverted here
    in >> packet.localUID;
    return in;
}
