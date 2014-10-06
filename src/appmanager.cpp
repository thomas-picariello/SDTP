#include "appmanager.h"

AppManager::AppManager(QObject* parent):
    QObject(parent)
{

}

AbstractApp* AppManager::getApp(AppUID localUID) const{
    return m_LocalAppsRegister.value(localUID, NULL);
}

AppUID AppManager::getLocalAppUID(AbstractApp* app) const{
    return m_LocalAppsRegister.key(app);
}

AppUID AppManager::getLocalAppUID(AppUID distantUID) const{
    return m_AppConnectionsTable.key(distantUID);
}

AppUID AppManager::getDistantAppUID(AppUID localUID) const{
    return m_AppConnectionsTable.value(localUID);
}

AppUID AppManager::getDistantAppUID(AbstractApp* app) const{
    return getDistantAppUID(getLocalAppUID(app));
}

bool AppManager::isAppRegistered(AbstractApp* app) const{
    return (m_LocalAppsRegister.key(app).type() != Undefined);
}

bool AppManager::isLocalAppConnected(AppUID localUID) const{
    return m_AppConnectionsTable.contains(localUID);
}

void AppManager::readIncommingData(QByteArray &data){
    Packet packet;
    QDataStream (&data, QIODevice::ReadOnly) >> packet;
    switch(packet.command){
    case StartAppCommand:
        emit startAppFor(packet.distantUID);
        break;
    case AppStartedSignal:
        registerConnection(packet.localUID, packet.distantUID);
        break;
    case AppClosedSignal:
        emit distantAppUnregistered(getLocalAppUID(packet.distantUID));
        unregisterApp(packet.distantUID);
        break;
    default:
        emit error(UnknownCommand);
    }
}

void AppManager::requestPartnerApp(AppUID localAppUID){
    sendPacket(StartAppCommand, localAppUID, AppUID());
}

void AppManager::registerApp(AppUID localAppUID, AbstractApp *app){
    m_LocalAppsRegister.insert(localAppUID, app);
    //TODO: remove debug localhost default loopback
    //m_AppConnectionsTable.insert(localAppUID, localAppUID);
}

void AppManager::unregisterApp(AppUID localAppUID){
    m_AppConnectionsTable.remove(localAppUID);
    m_LocalAppsRegister.remove(localAppUID);
}

bool AppManager::registerConnection(AppUID localUID, AppUID distantUID){
    bool appRegistered = m_LocalAppsRegister.value(localUID, NULL) != NULL;
    if(appRegistered && !m_AppConnectionsTable.contains(localUID)){
        m_AppConnectionsTable.insert(localUID, distantUID);
        sendPacket(AppStartedSignal, localUID, distantUID);
    }
    return appRegistered;
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
    in >> packet.distantUID;   //distant and local UID are inverted here
    in >> packet.localUID;
    return in;
}
