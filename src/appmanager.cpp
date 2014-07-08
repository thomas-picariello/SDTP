#include "appmanager.h"

void AppManager::readIncommingData(QByteArray &data){
    Packet packet;
    QDataStream (&data, QIODevice::ReadOnly) >> packet;
    qDebug()<<packet.command << packet.distantUID.type() << packet.distantUID.instanceID();
    switch(packet.command){
    case StartAppCommand:
        //emit startApp(packet.distantUID.type());
        break;
    case AppRegisterCommand:
        registerConnection(packet.localUID, packet.distantUID);
        break;
    case AppRegisteredSignal:

        break;
    case AppUnregisteredSignal:
        emit distantAppUnregistered(getLocalAppUID(packet.distantUID));
        unregisterApp(packet.distantUID);
        break;
    default:
        emit error(UnknownCommand);
    }
}

void AppManager::requestPartnerApp(AppUID localAppUID){
    Packet packet(StartAppCommand, localAppUID, AppUID());
    QByteArray data;
    QDataStream(&data, QIODevice::WriteOnly) << packet;
    emit sendData(TCP, data);
}

void AppManager::registerApp(AppUID localAppUID, AbstractApp *app){
    m_LocalAppsRegister.insert(localAppUID, app);
}

void AppManager::unregisterApp(AppUID localAppUID){
    m_AppConnectionsTable.remove(localAppUID);
    m_LocalAppsRegister.remove(localAppUID);
}

void AppManager::registerConnection(AppUID localAppUID, AppUID distantAppUID){
    m_AppConnectionsTable.insert(localAppUID, distantAppUID);
}

QDataStream& operator<<(QDataStream &out, const AppManager::Packet &packet){
    out << static_cast<quint8>(packet.command);
    out << packet.distantUID;   //distant and local UID inverted here
    out << packet.localUID;     //              ''
    return out;
}

QDataStream& operator>>(QDataStream &in, AppManager::Packet &packet){
    quint8 cmd;
    in >> cmd;
    packet.command = static_cast<AppManager::Command>(cmd);
    in >> packet.localUID;
    in >> packet.distantUID;
    return in;
}
