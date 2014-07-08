#ifndef APPMANAGER_H
#define APPMANAGER_H

#include <QObject>
#include <QMap>

#include "typesenums.h"
#include "abstractapp.h"
#include "appuid.h"

class AppManager : public QObject
{
    Q_OBJECT
public:
    Q_ENUMS(AppType)

    enum Command: quint8{
        StartAppCommand = 0x00,
        AppRegisterCommand = 0x01,
        AppRegisteredSignal = 0x02,
        AppUnregisteredSignal = 0x03,

        InvalidCommand = 0xFF
    };
    Q_ENUMS(Command)

    enum Error{
        PacketCorrupted,
        UnknownCommand
    };
    Q_ENUMS(Error)

    struct Packet{
        Packet(){}
        Packet(Command cmd, AppUID local, AppUID distant): command(cmd), localUID(local), distantUID(distant){}
        Command command;
        AppUID localUID;
        AppUID distantUID;
    };

    explicit AppManager(QObject* parent = 0): QObject(parent){}

    AbstractApp* getApp(AppUID uid)const            { return m_LocalAppsRegister.value(uid, NULL); }
    AppUID getLocalAppUID(AbstractApp* app) const   { return m_LocalAppsRegister.key(app); }
    AppUID getLocalAppUID(AppUID distantAppUID)const{ return m_AppConnectionsTable.key(distantAppUID); }
    AppUID getDistantAppUID(AppUID localAppUID)const{ return m_AppConnectionsTable.value(localAppUID); }
    AppUID getDistantAppUID(AbstractApp* app)const  { return getDistantAppUID(getLocalAppUID(app)); }
    bool isAppRegistered(AbstractApp* app)const     { return (m_LocalAppsRegister.key(app).type() != Undefined); }
    bool isAppConnected(AppUID distantAppUID)const  { return (m_AppConnectionsTable.key(distantAppUID).type() != Undefined); }

public slots:
    void readIncommingData(QByteArray &data);
    void requestPartnerApp(AppUID localAppUID);
    void registerApp(AppUID localAppUID, AbstractApp *app);
    void unregisterApp(AppUID localAppUID);

signals:
    void distantAppUnregistered(AppUID distantUID);
    void error(Error);
    void sendData(LinkType linkType, QByteArray& data);
    void startApp(AppType appType);


private:
    QMap<AppUID, AppUID> m_AppConnectionsTable;
    QMap<AppUID, AbstractApp*> m_LocalAppsRegister;

    void registerConnection(AppUID localAppUID, AppUID distantAppUID);

    Q_DISABLE_COPY(AppManager)
};

QDataStream& operator<<(QDataStream &out, const AppManager::Packet &packet);
QDataStream& operator>>(QDataStream &in, AppManager::Packet &packet);

#endif // APPMANAGER_H
